
//https://programmers.co.kr/learn/courses/30/lessons/1833
// 전역 변수를 정의할 경우 함수 내에 초기화 코드를 꼭 작성해주세요.
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

bool cmp(vector <int> a, vector <int> b)
{
    return a[0]<b[0];
}
int solution(int n, vector<vector<int>> data) {
    int answer = 0;   
    sort(data.begin(),data.end(),cmp);
    
    int x,y;
    int mx,my;
    int px,py;
    int tx,ty;
    for(int i=0; i<n; i++)
    {
        x=data[i][0];
        y=data[i][1];
        for(int j=i+1; j<n; j++)
        {
            tx=data[j][0];
            ty=data[j][1];
            
            nx=min(x,tx);
            ny=min(y,ty);
            mx=max(x,tx);
            my=max(y,ty);
            
            if(nx==mx||ny==my) continue;
            
            int sw=0;
            int jx,jy;
            for(int k=i+1; k<j; k++)
            {
                jx=data[k][0];
                jy=data[k][1];
                if(jx>mx&&jx<px&&jy>my&&jy<py)
                {
                    sw=1;
                    break;
                }
            }
            e
            if(sw==0) answer++;
        }
    }
    
    return answer;
}

https://kwonik2304.tistory.com/32
https://programmers.co.kr/learn/courses/30/lessons/1833