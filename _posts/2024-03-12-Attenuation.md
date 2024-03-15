---
layout: post
title: "Attenuation"
categories: graphics
---

## Overview

<!-- begin_excerpt -->
빛은 이동한 거리에 따라 그 세기가 감소한다. 수성과 금성이 지구와 화성보다 뜨거운 이유다.
<!-- end_excerpt -->
<br>
태양으로부터 발생한 빛은 공간을 지나면서 흡수되거나 산란하여 거리가 멀어질수록 그 세기가 감소한다. <br>
이를 기술적으로 빛의 감쇠(light attenuation)라고 부른다. <br>
<br>
빛이 통과하는 공간에 따라 감쇠하는 양이 다르지만 보통 공기 중에선 [이동한 거리의 제곱에 반비례](https://en.wikipedia.org/wiki/Attenuation)한다고 알려져 있다. <br>

이는 식 \\(attenuation = \frac 1 {d^2}\\)로 쓸 수 있는데 거리 d 값이 증가함에 따라 급격한 감쇠를 보여준다.<br>

또한 위 식은 1을 기점으로 거리 d가 1보다 작다면 빛은 감쇠하지 않고 크다면 감쇠하여 0.0에서 1.0의 범위를 가진다.<br>

> <font size="2"> 
> 감쇠 값은 1보다 큰 값은 1로 맞추고 거리가 멀어질수록 0에 무한히 가까워지지만 0은 되지 않는다.
> </font>


## Attenuation Equation Of OpenGL

빛의 감쇠는 아래처럼 단순히 어떤 물체의 원본 색상을 얼마의 비율로 픽셀에 적용할지 계산 하는데 사용 한다.<br>

```
to_light = u_Light_position - v_Vertex;
d = length( to_light );
attenuation = clamp( 10.0 / pow(d, 2), 0.0, 1.0);
// ...
color = attenuation * (ambient_color + diffuse_color + specular_color);
```

하지만 물리적인 감쇠식을 그대로 사용하기에는 거리에 따른 매우 빠른 감쇠로 다양한 설정을 하기 어렵다. <br>

OpenGL Light 모델은 거리를 변수로 한 2차 방정식의 해로 나눈 값으로 빛의  감쇠를 계산 한다.<br>

<span style="font-size: x-large;">
\\( attenuation = \frac {1.0} {a * d^2 + b * d + c} \\)
</span>

여기서, 2차 방정식의 계수 a, b, c는 각각 이차(quadratic), 선형(linear), 상수(constant) 조절 값이다. 

아래 그래프에서 a, b를 조절해가며 감쇠 값의 변화를 확인 할 수 있다.

<html>
<head>
    <title>Attenuation Graph</title>
    <style>
        canvas {
            max-width: 400px;
            max-height: 200px;
            margin: 0 auto;
            display: block;
        }
        .slider-container {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin-top: 20px;
        }
        .slider {
            width: 300px;
            margin-top: 10px;
            text-align: left;
            margin-left: -100px; /* 좌측으로 이동 */
        }
        .fixed-value {
            margin-top: 10px;
            margin-left: -361px;
        }
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <canvas id="myChart" width="400" height="200"></canvas>
    <div class="slider-container">
        <div class="slider">
            <label for="a">a:</label>
            <input type="range" id="a" name="a" min="0" max="10" step="0.1" value="1">
            <span id="aValue">1</span>
        </div>
        <div class="slider">
            <label for="b">b:</label>
            <input type="range" id="b" name="b" min="0" max="10" step="0.1" value="1">
            <span id="bValue">1</span>
        </div>
        <div class="fixed-value">
            c: 1.0
        </div>
    </div>

    <script>
        var ctx = document.getElementById('myChart').getContext('2d');
        var data = {
            labels: ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],
            datasets: [{
                data: [],
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                borderColor: 'rgba(255, 99, 132, 1)',
                borderWidth: 1
            }]
        };

        var myChart = new Chart(ctx, {
            type: 'line',
            data: data,
            options: {
                animation: {
                    duration: 0 // 애니메이션 비활성화
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 1.0
                    },
                    x: {
                        max: 10.0
                    }
                },
                plugins: {
                    legend: {
                        display: false
                    }
                }
            }
        });

        updateGraph();

        document.getElementById('a').addEventListener('input', updateGraph);
        document.getElementById('b').addEventListener('input', updateGraph);

        function updateGraph() {
            var aValue = document.getElementById('a').value;
            var bValue = document.getElementById('b').value;

            document.getElementById('aValue').textContent = aValue;
            document.getElementById('bValue').textContent = bValue;

            var newData = [];
            for (var i = 0; i <= 10; i++) {
                var d = i;
                var attenuation = 1 / (parseFloat(aValue) * Math.pow(d, 2) + parseFloat(bValue) * d + 1.0);
                newData.push(attenuation);
            }

            myChart.data.datasets[0].data = newData;
            myChart.update(); 
        }
    </script>
</body>
</html>
