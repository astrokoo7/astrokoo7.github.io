#include <immintrin.h>
#include <iostream>

static __m128 sdp2(__m128 a, __m128 b, __m128 c, __m128& x)
{
    // 함수 내용은 이전과 동일
}

int main() {
    // 4개의 2차 방정식을 동시에 계산
    __m128 a = _mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);  // a 계수
    __m128 b = _mm_set_ps(2.0f, 3.0f, 4.0f, 5.0f);  // b 계수
    __m128 c = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);  // c 계수
    __m128 x = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);  // x 값

    __m128 result = sdp2(a, b, c, x);

    // 결과 출력
    float results[4];
    _mm_store_ps(results, result);

    for (int i = 0; i < 4; i++) {
        std::cout << "Result " << i << ": " << results[i] << std::endl;
    }

    return 0;
}