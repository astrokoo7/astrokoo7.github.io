using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.HighDefinition;

public class Floater : MonoBehaviour
{
    public Rigidbody rb;
    public float depthBefSub;
    public float displacementAmt;
    
    public int floaters;
    // 물에서의 드래그 계수
    public float waterDrag;
    // 물에서의 각도 드래그 계수
    public float waterAngularDrag;
    // 물 표면 관리 컴포넌트 참조
    public WaterSurface water;

    // 물 표면을 검색하는 파라미터 저장
    WaterSearchParameters Search;
    // 물 표면 검색 결과 저장
    WaterSearchResult SearchResult;

    private void FixedUpdate()
    {
        // 분산된 중력 적용
        rb.AddForceAtPosition(Physics.gravity / floaters, transform.position, ForceMode.Acceleration);

        // 물 표면에 투영하기 위한 검색 파라미터 설정
        Search.startPositionWS = transform.position;

        // 물 표면에 포인트 투영하고 결과 받기
        water.ProjectPointOnWaterSurface(Search, out SearchResult);

        // 객체가 물 표면 아래에 있을 경우
        if (transform.position.y < SearchResult.projectedPositionWS.y)
        {
            // 잠수 깊이에 따른 변위 배수 계산
            float displacementMulti = Mathf.Clamp01((SearchResult.projectedPositionWS.y - transform.position.y) / depthBefSub) * displacementAmt;
            
            // 부력 위로 적용
            rb.AddForceAtPosition(new Vector3(0f, Mathf.Abs(Physics.gravity.y) * displacementMulti, 0f), transform.position, ForceMode.Acceleration);

            // 물 드래그 힘 적용 (속도에 따라)
           rb.AddForce(displacementMulti * -rb.velocity * waterDrag * Time.fixedDeltaTime, ForceMode.VelocityChange);

            // 물 각도 드래그 토크 적용 (각도 속도에 따라)
           rb.AddTorque(displacementMulti * -rb.angularVelocity * waterAngularDrag * Time.fixedDeltaTime, ForceMode.VelocityChange);
        }
    }
}



using UnityEngine;
using UnityEngine.Rendering.HighDefinition;


public class Floater2 : MonoBehaviour
{
    public int floaters = 4; // 부력 포인트 수
    public float buoyancyForce = 10.0f; // 부력 계수
    public float dragCoefficient = 0.1f; // 물 저항 계수
    public float angularDragCoefficient = 0.05f; // 회전 저항 계수
    public float depthBeforeSubmersion = 1.0f; // 물에 잠기기 시작하는 깊이
    public float dampingFactor = 0.2f; // 감쇠 계수로, 통통 튀는 현상 방지

    private Vector3 velocity; // 물체의 현재 속도
    private Vector3 angularVelocity; // 물체의 현재 회전 속도
    public float gravity = -9.81f; // 중력 가속도

    public WaterSurface water;
    
    // 물 표면을 검색하는 파라미터 저장
    WaterSearchParameters Search;
    // 물 표면 검색 결과 저장
    WaterSearchResult SearchResult;    
    
    void Update()
    {
        // 현재 위치 가져오기
        Vector3 currentPosition = transform.position;

        Search.startPositionWS = transform.position;
        
        water.ProjectPointOnWaterSurface(Search, out SearchResult);

        
        // 물 속에 잠긴 경우
        if (transform.position.y < SearchResult.projectedPositionWS.y)
        {
            // 잠긴 정도에 따라 부력 비율 계산 (0에서 1 사이)
            float submersionRatio = Mathf.Clamp01(SearchResult.projectedPositionWS.y - transform.position.y / depthBeforeSubmersion);

            // 부력 계산
            Vector3 buoyancy = new Vector3(0, buoyancyForce * submersionRatio, 0);
            velocity += buoyancy * Time.deltaTime;

            // 물 저항 계산 (속도와 반대 방향으로 저항력)
            Vector3 drag = -velocity * dragCoefficient * submersionRatio;
            velocity += drag * Time.deltaTime;
            


            // 회전 저항 계산 (회전 속도와 반대 방향으로 저항력)
            Vector3 angularDrag = -angularVelocity * angularDragCoefficient * submersionRatio;
            angularVelocity += angularDrag * Time.deltaTime;
        }
        else
        {
            // 물 밖에 있는 경우 중력 적용
            Vector3 gravityForce = new Vector3(0, gravity, 0);
            velocity += gravityForce * Time.deltaTime;
        }

        velocity *= (1 - dampingFactor * Time.deltaTime);        
        
        // 위치와 회전 업데이트
        currentPosition += velocity * Time.deltaTime;
        transform.position = currentPosition;

        // 회전 업데이트
        Vector3 currentRotation = transform.eulerAngles;
        currentRotation += angularVelocity * Time.deltaTime;
        transform.eulerAngles = currentRotation;
    }
}


void UpdateShaderVariablesWater(WaterSurface currentWater, int surfaceIndex, ref ShaderVariablesWater cb)

void EvaluateWaterAdditionalData(float3 positionOS, float3 transformedPosition, float3 meshNormalOS, out WaterAdditionalData waterAdditionalData)
_WaterFoamRegionResolution

waterutilities.hlsl:654






















void mainImage( out vec4 O, vec2 U )
{
	vec2 R = iResolution.xy, 
         M = vec2(.0,.5);
    O = vec4(0);
    U = U  / R.y; 
 
    float  S = 1.,                                // invariant scaling factor
           V = 10.,                       // [m/s] boat spead
           //L = 2500.,                           // [m] screen->world scaling
           L = 1.,                           // [m] screen->world scaling
           W =  128./S*exp2(5.*M.y),              // width of spectrum of emission
           dx = 2./R.y;                           // pixel size. Multiply for more FPS
    float x = 0.;
    for (; x<=5.; x += dx ) {         // sum pulse emitted at each past location
        vec2 P = U  - vec2(x,0.); // pixel position in past boat frame
        float l = length(P)*L, 
              k = 6.283/(2.*l),                   // k=2pi/L, energy at l=L/2 since Cg = 1/2 Cphi
              t = x*L/V,                          // time where boat was at x (approx)
              a = 3.1416 - sqrt(9.81*k) *t,       // cos(kl-wt), kl=pi, w=sqrt(gk)  
              v = l/W;                       // ( Dispersion relations: https://en.wikipedia.org/wiki/Dispersion_(water_waves)
       // O +=  cos(a) / l  * exp(-.5*v*v);         // spectrum of wave emission by the boat
        
        //O += (l * .0000001);
        //break;
    }
    
    O += dx;
    //O += step(M.x, -0.01);;
    
    
    //O += U.x*0.001;
    //O = .5* O*dx *1.7e3;
}



