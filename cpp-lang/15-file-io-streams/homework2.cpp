#include <iostream>
#include <fstream>
#include <cstring>

struct Student {
    char name[32];
    int age;
    double score;
};

int main() {
    std::string binfile = "students.bin";
    
    // 이진 파일에 구조체 배열 저장
    std::cout << "이진 파일에 학생 정보 저장...\n";
    
    Student students[] = {
        {"김철수", 25, 85.5},
        {"이영희", 24, 92.0},
        {"박민지", 26, 78.5},
        {"최준호", 23, 88.0}
    };
    
    std::ofstream outfile(binfile, std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "파일 열기 실패: " << binfile << "\n";
        return 1;
    }
    
    int count = 4;
    outfile.write(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (int i = 0; i < count; i++) {
        outfile.write(reinterpret_cast<char*>(&students[i]), sizeof(Student));
    }
    outfile.close();
    
    // 이진 파일에서 구조체 배열 읽기
    std::cout << "\n이진 파일에서 학생 정보 읽기:\n";
    std::cout << "================================\n";
    
    std::ifstream infile(binfile, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "파일 열기 실패: " << binfile << "\n";
        return 1;
    }
    
    int readcount;
    infile.read(reinterpret_cast<char*>(&readcount), sizeof(readcount));
    
    std::cout << "읽은 학생 수: " << readcount << "\n\n";
    
    for (int i = 0; i < readcount; i++) {
        Student s;
        infile.read(reinterpret_cast<char*>(&s), sizeof(Student));
        
        if (infile.good()) {
            std::cout << "학생 " << (i + 1) << ":\n";
            std::cout << "  이름: " << s.name << "\n";
            std::cout << "  나이: " << s.age << "\n";
            std::cout << "  점수: " << s.score << "\n\n";
        }
    }
    
    // 파일 위치 확인
    std::cout << "파일 위치 정보:\n";
    std::cout << "현재 위치: " << infile.tellg() << " 바이트\n";
    
    infile.close();
    
    return 0;
}
