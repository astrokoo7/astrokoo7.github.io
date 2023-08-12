---
layout: post
title: "Perspective Projection"
categories: graphics
---

## overview

<!-- begin_excerpt -->

시야 기준 3D 가상 공간을 표현하는 데카르트 좌표계상 한점 \\( (X_{eye}, Y_{eye}, Z_{eye}) \\)은 깊이 값 \\( Z_{eye} \\)를 차수 \\( W_{clip} \\)로 지정하여 동차좌표계(Clip Space)상 한점 \\( (X_{clip}, Y_{clip}, Z_{clip}, W_{clip}) \\)로 변환 후 다시 데카르트 좌표계로 역변환(Perspective Divide)하여 데카르트 좌표계상 원근감을 생성한다.

<!-- end_excerpt -->

이는 직교 투영(orthographic projection)과 대비하여 생각해 볼 수 있는데 원근 투영(Perspective Projection)은 아래의 변환 과정을 거치며

* View Coordinate → Homogeneous Coordinate → Normalize Device Coordinate

이에 반하여 직교 투영(orthographic projection)은 NDC 좌표계로 바로 변환된다.

* View Coordinate → Normalize Device Coordinate

## clip space

View Coordinate는 Normalize Device Coordinate로 변환전 Clip Coordinate로 변환 과정을 거치는데 이때 \\( [-W_{clip}, W_{clip}] \\) 범위를 넘는 Vertex는 버려진다.


<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/ndc_1_hbjofy.png" width="50%">
  <figcaption></figcaption>
</div>
</figure>



> <font size="2"> 
> Note  <br>
> 1. Clip Space는 원근 투영일 경우 Homogeneous Coordinate로 표현된다. <br>
> 2. 직교투영도 Clip Coordinate 변환 과정을 거치며 같은 룰이 적용되어 범위를 벗어나면 버려진다.   <br>
> 3. 직교투영도 Perspective Divide를 하지만  \(W_{\text{clip}}\)가 1로 설정되어 사실상 무시 된다.  <br>
> 4. Clip Coordinate 변환 후 범위 밖의 Vertex는 버려져 Perspective Divide 비용을 줄였다.  <br>
> 5. Perspective Divide는 GPU에 의해 자동 수행된다.
> </font> 

```
void main()
{
   gl_Position = vec4(0.5, 0.5, 0.5, 1.0);
}
```
위의 버텍스는 \\( W_{clip} \leq (X_{clip}, Y_{clip}, Z_{clip}) \leq W_{clip} \\)를 만족하여 화면에 출력된다.
> <font size="2"> 
> 버텍스 쉐이더가 출력하는 gl_Position은 Clip Coordinate 이다.
> </font> 

## perspective projection matrix

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/projectionOpenGL2.png" width="50%">
  <figcaption>녹색의 near plane으로 절두체 공간의 점이 투영된다.</figcaption>
</div>
</figure>

원근 투영 행렬은 View Coordinate를 Clip Coordinate로 변환해주는 행렬로 각각의 좌표값을 구해보자.

> <font size="2"> 
> 원근 투영 행렬일때 Clip Coordinate는 Homogeneous Coordinate가 된다.
> </font> 

* \\( W_{clip} \\) 구하기

동차 좌표계(Homogeneous Coordinate)를 사용한 원근감은 \\( Z_{eye} \\)를 \\( W_{clip} \\)로 지정하여 생성 후  Perspective Divide를 통해 반영하여 \\( W_{clip} \\) = \\( - Z_{eye} \\) 가 된다.

이를 행렬식으로 표현하면 다음과 같다.

$$
\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
\begin{pmatrix}
. & . & . & . \\
. & . & . & . \\
. & . & . & . \\
0 & 0 & -1 & {0} \\
\end{pmatrix}

\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}

$$

> <font size="2"> 
> 카메라 좌표계는 오른손 좌표계를 사용하여 View Coordinate에서 카메라가 바라보는 Z축은 음수 방향이다. 
> </font> 

* \\( X_{clip}과 Y_{clip} \\) 구하기

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/triangle.png" width="65%">
  <figcaption></figcaption>
</div>
</figure>

원근 투영은 닮은 삼각형 성질에 따라 \\(X_{eye}\\)를 \\(X_{proj}\\)로 투영하면 다음으로 정리된다.

\\( \frac {X_{proj}} {Z_{proj}} = \frac {X_{eye}} {Z_{eye}} \\)

\\( X_{proj} = \frac {X_{eye} Z_{proj}} {Z_{eye}} \\)

또한 \\( X_{proj} \\)는 범위 \\( [X_{left}, X_{right}] \\)를 만족해야 한다.

 \\( l \leq X_{proj} \leq r \\)

> <font size="2"> 
> 아직 view cooridinates 이다.
> </font>

위 범위를 만족하는 \\( X_{proj} \\)가 Normalize Device Coordinates로 최종 변환한다고 생각해보자.

NDC는 [-1, 1]의 범위를 가지니 위 식을 [-1, 1]의 범위로 변환하면 \\( W_{clip} \\)를 유추할 수 있다.

> <font size="2"> 
> clip coordinates는 \( -w \leq X_{clip} \leq w \)로 w로 나누면 ndc가 된다. 반대로 ndc를 구한 후 w를 곱하면 \( X_{clip} \)를 구할 수 있다.
> </font>

\\( l \leq X_{proj} \leq r \\)

\\( 0 \leq X_{proj} -l \leq r - l \\)

\\( 0 \leq \frac {(X_{proj} -l)} {r - l} \leq 1 \\)

\\( 0 \leq 2 \frac {(X_{proj} -l)} {r - l} \leq 2 \\)

\\( -1 \leq 2 \frac {(X_{proj} -l)} {r - l} - 1 \leq 1 \\)

\\( -1 \leq \frac {2 X_{proj}} {r - l} - \frac {r + l} {r - l} \leq 1 \\)

여기서 \\( X_{proj} = \frac {X_{eye} Z_{proj}} {Z_{eye}} \\)를 대입하면 

\\( -1 \leq \frac {2 X_{eye} Z_{proj}} {Z_{eye} ({r - l})} - \frac {r + l} {r - l} \leq 1 \\)

\\(Z_{eye}\\)는 \\(W_{clip}\\)이고 \\( Z_{proj} \\)는 Near로 상수이다.

\\( -1 \leq \frac {2 X_{eye} Near} {W_{clip} ({r - l})} - \frac {r + l} {r - l} \leq 1 \\)

이제 \\( W_{clip} \\)를 곱하여 Clip Cooridates를 구해보자.

\\( -W_{clip} \leq \frac {2 X_{eye} Near} {(r - l)} - \frac {W_{clip} (r + l)} {r - l} \leq W_{clip} \\)

정리하면 아래와 같고 

\\( X_{clip} = \frac {2 X_{eye} Near} {(r - l)} - \frac {W_{clip} (r + l)} {r - l} \\)


Perspective Projection Matrix는 Clip Coordinates로 변환이니 위 식을 행렬식으로 변환하면 다음과 같다.

$$
\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
\begin{pmatrix}
\frac {2 n} {(r - l)} & 0 & \frac {r + l} {r - l} & 0 \\
. & . & . & . \\
. & . & . & . \\
. & . & . & . \\
\end{pmatrix}

\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}
$$

> <font size="2"> 
> \( W_{clip} \)과 \( Z_{eye} \)는 같다.
> </font>


\\( Y_{clip} \\)은 left, right를 bottom, top로 바꿔주면 된다.


$$
\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
\begin{pmatrix}
. & . & . & . \\
0 & \frac {2 n} {(t - b)} & \frac {t + b} {t - b} & 0 \\
. & . & . & . \\
. & . & . & . \\
\end{pmatrix}

\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}
$$

* \\( Z_{clip}\\) 구하기

원근감을 생성하기 위해 닮은 삼각형 비율을 이용했던 \\( X_{clip}과 Y_{clip} \\)와는 달리 \\( Z_{clip}\\)은 다른 값에 영향을 받지 않는다. 따라서 영향 받지 않는 \\( X_{eye}와 Y_{eye} \\)를 제외한 모르는 미지수 두개의 A, B를 계수로 식을 세운 뒤 \\( Z_{eye}\\)에 대한 \\( Z_{clip}\\)의 방적식을 만든뒤 알려진 상수로 방정식의 A, B 구하여 해를 찾는 식이다. 

이를 수식으로 표현하면 아래와 같다.


$$
\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
\begin{pmatrix}
. & . & . & . \\
. & . & . & . \\
0 & 0 & A & B \\
0 & 0 & -1 & {0} \\
\end{pmatrix}

\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}

$$

\\( Z_{ndc} = \frac {Z_{ndc}} {W_{clip}} =  \frac { A \cdot {Z_{eye}}+ B \cdot {W_{eye}}} {-Z_{eye}} \\) 

여기서 Eye Coordinates 상 \\(W_{eye}\\)는 1 이니 

\\( Z_{ndc} = \frac { A \cdot {Z_{eye}}+ B} {-Z_{eye}} \\) 이고

\\(Z_{eye}와 Z_{ndc}\\)는 -Near일때 -1를 가지고 -far일때 1이니 각각에 대해 방정식을 만들면 A와 B를 구할 수 있다.

\\(\frac {-ANear + B} n = -1\\) 와 

\\(\frac {-AFar + B} f = 1\\) 를 A와 B로 정리하면 

\\(A = - \frac {f + n} {f - n} \\) 이 되고

\\(B = - \frac {2fn} {f - n} \\)이 된다.

$$
\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
\begin{pmatrix}
. & . & . & . \\
. & . & . & . \\
. & . &  - \frac {f + n} {f - n} & - \frac {2fn} {f - n}  \\
0 & 0 & -1 & {0} \\
\end{pmatrix}

\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}

$$

앞서 아래의 식에서 보았든 \\( Z_{eye}에서 Z_{ndc} \\)로의 변환은 선형 방정식이 아니다.

\\( Z_{ndc} = \frac { A \cdot {Z_{eye}}+ B \cdot {W_{eye}}} {-Z_{eye}} \\) 

따라서 \\( Z_{ndc} \\) 는 아래의 그래프 형태를 가지며 이는 Near Plane에서 매우 높은 해상도를 가지지만 far Plane에 가까워 질수록 낮은 해상도를 가지게 됨을 알 수 있다. 이는 먼거리에서 z-fighting이 발생할 여지가 크며 먼거리의 가까운 \\(Z_{eye}\\) 간 식별이 잘 안되는 문제를 가지고 있다.

<figure>
<div style="text-align:center;">
  <img src="{{ site.url }}{{ site.baseurl }}/assets/images/gl_projectionmatrix07.png" width="90%">
  <figcaption></figcaption>
</div>
</figure>

완성된 perspective projection matrix는 다음과 같다.


$$
\begin{pmatrix}
X_{clip} \\
Y_{clip} \\
Z_{clip} \\
W_{clip} \\
\end{pmatrix}
=
\begin{pmatrix}
\frac {2 n} {(r - l)} & 0 & \frac {r + l} {r - l} & 0 \\
0 & \frac {2 n} {(t - b)} & \frac {t + b} {t - b} & 0 \\
0 & 0 &  - \frac {f + n} {f - n} & - \frac {2fn} {f - n}  \\
0 & 0 & -1 & {0} \\
\end{pmatrix}

\begin{pmatrix}
X_{eye} \\
Y_{eye} \\
Z_{eye} \\
W_{eye} \\
\end{pmatrix}

$$
