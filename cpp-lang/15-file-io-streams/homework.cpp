#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int main() {
    std::string filename = "data.txt";
    
    // 파일에 데이터 쓰기
    std::cout << "파일에 데이터 쓰기...\n";
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "파일 열기 실패: " << filename << "\n";
        return 1;
    }
    
    outfile << "이름,나이,점수\n";
    outfile << "김철수,25,85\n";
    outfile << "이영희,24,92\n";
    outfile << "박민지,26,78\n";
    outfile.close();
    
    // 파일에서 데이터 읽기
    std::cout << "\n파일에서 데이터 읽기:\n";
    std::cout << "================================\n";
    
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "파일 열기 실패: " << filename << "\n";
        return 1;
    }
    
    std::string line;
    int linenum = 0;
    while (std::getline(infile, line)) {
        std::cout << "줄 " << (++linenum) << ": " << line << "\n";
    }
    
    infile.close();
    
    // 파일 상태 확인
    std::cout << "\n파일 상태:\n";
    infile.open(filename);
    if (infile.good()) {
        std::cout << "✓ 파일 상태: 정상 (good)\n";
    }
    
    // 파일 끝까지 읽기
    char c;
    int charcount = 0;
    while (infile.get(c)) {
        charcount++;
    }
    std::cout << "✓ 파일 크기: " << charcount << " 바이트\n";
    
    if (infile.eof()) {
        std::cout << "✓ 파일 끝 도달 (EOF)\n";
    }
    
    infile.close();
    
    return 0;
}
