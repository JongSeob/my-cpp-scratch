#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstdint>  // For uint32_t type
#include "MemSegment.h" // MemSegment 클래스 헤더

// 16x16 YCbCr 4:2:0 JPEG 비트스트림을 생성하여 MemSegmentList에 저장하고 반환하는 함수
MemSegmentList GenTestBitstream() {
    std::cout << "Creating test JPEG bitstream with 16x16 YCbCr 4:2:0 format...\n";
    
    // Y, Cb, Cr 컴포넌트를 위한 MemSegment 객체 생성
    // 4:2:0 서브샘플링에서:
    // - Y는 16x16 픽셀 = 4개의 8x8 블록 = 256개 픽셀
    // - Cb, Cr은 각각 8x8 픽셀 = 1개의 8x8 블록 = 64개 픽셀
    MemSegment* y_segment = new MemSegment("Y_COEFFICIENTS");
    MemSegment* cb_segment = new MemSegment("Cb_COEFFICIENTS");
    MemSegment* cr_segment = new MemSegment("Cr_COEFFICIENTS");
    
    // DCT 계수를 저장할 MemSegmentList 생성
    MemSegmentList segment_list;
    
    // 16x16 Y 컴포넌트 (4개의 8x8 DCT 블록)
    // 블록 배치:
    // [블록0] [블록1]
    // [블록2] [블록3]
    const int NUM_Y_BLOCKS = 4;  // 16x16 이미지는 Y 컴포넌트에 4개의 8x8 블록
    
    // Y 컴포넌트 coefficient 생성
    for (int block = 0; block < NUM_Y_BLOCKS; ++block) {
        for (int coef = 0; coef < 64; ++coef) {
            if (coef == 0) {
                // DC coefficient (블록의 평균 휘도)
                // 서로 다른 블록에 다른 값을 할당하여 이미지 패턴 생성
                (*y_segment)[block * 64 + coef] = 1024 - (block * 200);
            } 
            else {
                // AC coefficient - 저주파 부분에 약간의 값을 할당하고, 고주파는 대부분 0으로 설정
                // 지그재그 인덱스 기준으로 낮은 인덱스(저주파)에만 값 할당
                if (coef < 10) {
                    int value = 0;
                    switch (block) {
                        case 0: // 왼쪽 상단 블록 - 수평 에지
                            if (coef == 1 || coef == 8) value = 30;
                            else value = 5 - (coef / 2);
                            break;
                        case 1: // 오른쪽 상단 블록 - 수직 에지
                            if (coef == 1 || coef == 2) value = 35;
                            else value = 5 - (coef / 2);
                            break;
                        case 2: // 왼쪽 하단 블록 - 대각선 에지
                            if (coef == 1 || coef == 9) value = 25;
                            else value = 5 - (coef / 2);
                            break;
                        case 3: // 오른쪽 하단 블록 - 평탄한 영역
                            value = 2 - (coef / 5);
                            break;
                    }
                    (*y_segment)[block * 64 + coef] = value;
                } 
                else {
                    // 고주파 성분은 대부분 0
                    (*y_segment)[block * 64 + coef] = (coef % 23 == 0) ? 1 : 0;
                }
            }
        }
    }
    
    // Cb 컴포넌트 coefficient 생성 (8x8 픽셀, 1개의 블록)
    for (int coef = 0; coef < 64; ++coef) {
        if (coef == 0) {
            // DC coefficient (색상 정보)
            (*cb_segment)[coef] = 512;  // 중간 값 (0이 무채색)
        } 
        else if (coef < 8) {  // 저주파 AC coefficient만 값 가짐
            // 약간의 푸른 색 그라디언트를 위한 값
            (*cb_segment)[coef] = (coef == 1) ? 30 : (coef == 2 ? 20 : (coef == 3 ? 10 : 0));
        } 
        else {
            (*cb_segment)[coef] = 0;  // 고주파는 0
        }
    }
    
    // Cr 컴포넌트 coefficient 생성 (8x8 픽셀, 1개의 블록)
    for (int coef = 0; coef < 64; ++coef) {
        if (coef == 0) {
            // DC coefficient (색상 정보)
            (*cr_segment)[coef] = 480;  // 중간 값 (0이 무채색)
        } 
        else if (coef < 8) {  // 저주파 AC coefficient만 값 가짐
            // 약간의 붉은 색 그라디언트를 위한 값
            (*cr_segment)[coef] = (coef == 1) ? 25 : (coef == 2 ? 15 : (coef == 8 ? 10 : 0));
        } 
        else {
            (*cr_segment)[coef] = 0;  // 고주파는 0
        }
    }
    
    // 가상의 JPEG 헤더 정보 생성 (실제 비트스트림이 아닌 메타데이터)
    // 이 정보는 실제 JPEG 파일에서 추출해야 하지만 예시로 생성
    MemSegment* header_segment = new MemSegment("JPEG_HEADER");
    
    // SOI 마커 (Start of Image) - 0xFFD8
    (*header_segment)[0].word = 0xFFD8;
    
    // APP0 마커 (JFIF 애플리케이션 마커) - 0xFFE0 + 길이 및 기본 데이터
    (*header_segment)[1].word = 0xFFE0;
    (*header_segment)[2].word = 0x0010;  // 길이 (16바이트)
    (*header_segment)[3].word = 0x4A46;  // "JF"
    (*header_segment)[4].word = 0x4946;  // "IF"
    (*header_segment)[5].word = 0x0001;  // 버전 1.0
    
    // DQT 마커 (Define Quantization Table) - 0xFFDB + 간단한 Y 양자화 테이블
    (*header_segment)[6].word = 0xFFDB;
    (*header_segment)[7].word = 0x0043;  // 길이 (67바이트)
    (*header_segment)[8].word = 0x0010;  // 테이블 ID 0 & 8비트 정밀도
    
    // SOF0 마커 (Start Of Frame, Baseline DCT) - 0xFFC0 + 프레임 정보
    (*header_segment)[9].word = 0xFFC0;
    (*header_segment)[10].word = 0x0011;  // 길이 (17바이트)
    (*header_segment)[11].word = 0x0800;  // 8비트 샘플 & 높이 상위 바이트
    (*header_segment)[12].word = 0x1000;  // 높이 하위 바이트 & 너비 상위 바이트
    (*header_segment)[13].word = 0x1003;  // 너비 하위 바이트 & 컴포넌트 수 (3)
    
    // DHT 마커 (Define Huffman Table) - 0xFFC4 + 기본 테이블 정보
    (*header_segment)[14].word = 0xFFC4;
    (*header_segment)[15].word = 0x00A2;  // 길이 (기본 테이블 크기)
    
    // SOS 마커 (Start of Scan) - 0xFFDA + 스캔 정보
    (*header_segment)[16].word = 0xFFDA;
    (*header_segment)[17].word = 0x000C;  // 길이 (12바이트)
    (*header_segment)[18].word = 0x0301;  // 컴포넌트 수 (3) & 첫 번째 컴포넌트 ID
    (*header_segment)[19].word = 0x0002;  // 두 번째 컴포넌트 ID & 세 번째 컴포넌트 ID
    (*header_segment)[20].word = 0x1100;  // 시작/종료 스펙트럼 위치 & 연속 근사
    
    // 모든 세그먼트를 리스트에 추가
    segment_list.AddSegment(header_segment);
    segment_list.AddSegment(y_segment);
    segment_list.AddSegment(cb_segment);
    segment_list.AddSegment(cr_segment);
    
    return segment_list;
}

int main() {
    // 16x16 YCbCr 4:2:0 테스트 JPEG 비트스트림 생성
    MemSegmentList jpeg_segments = GenTestBitstream();
    
    // 메모리 세그먼트 정보 출력
    std::cout << "\n===== JPEG Coefficient Data in Memory Segments =====\n";
    jpeg_segments.PrintAll();
    for (size_t i = 0; i < jpeg_segments.GetSize(); ++i) {
        const MemSegment* segment = jpeg_segments[i];
        std::string filename = segment->GetName();
        if (segment) {
            segment->SaveTo(filename + ".txt");
        }
    }
    return 0;
    
    // 요약 정보 출력
    std::cout << "\nJPEG Segment Summary:\n";
    std::cout << "Total segments: " << jpeg_segments.GetSize() << "\n";
    
    // 각 세그먼트 정보 표시
    for (size_t i = 0; i < jpeg_segments.GetSize(); ++i) {
        const MemSegment* segment = jpeg_segments[i];
        if (segment) {
            std::cout << "  Segment #" << i << " ('" << segment->GetName() 
                      << "'): " << segment->GetSize() << " words\n";
            
            // 첫 번째 세그먼트(헤더)의 마커 정보 표시
            if (i == 0) {
                std::cout << "    JPEG Markers:\n";
                for (size_t j = 0; j < std::min(segment->GetSize(), static_cast<size_t>(21)); j += 2) {
                    if ((*segment)[j].word >= 0xFF00) {
                        std::cout << "      Offset " << std::setw(2) << j 
                                  << ": Marker 0x" << std::hex << std::setw(4) << std::setfill('0')
                                  << (*segment)[j].word << std::dec << std::setfill(' ') << "\n";
                    }
                }
            }
            // Y/Cb/Cr 세그먼트의 DC 계수 정보 표시 (첫 번째 계수가 DC)
            else if (segment->GetSize() > 0) {
                std::cout << "    First coefficients (DC values):\n";
                
                // 각 블록의 첫 번째 계수(DC) 표시
                for (size_t j = 0; j < segment->GetSize() && j < 256; j += 64) {
                    std::cout << "      Block " << (j / 64) << " DC: " 
                              << std::setw(4) << (*segment)[j].word << "\n";
                }
            }
        }
    }
    
    // 메모리 누수 방지 - 할당한 세그먼트 해제
    for (size_t i = 0; i < jpeg_segments.GetSize(); ++i) {
        delete jpeg_segments[i];
    }
    
    return 0;
}