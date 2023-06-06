---
layout: post
title: "virtualization"
categories: computer
---

os 기능의 일부분으로 hardware virtualization이 제공 된다.

하드웨어와 직접 연결 된 호스트 os 외에 추가로 os를 구동하려면 

예전에는 vmware 같은 미들웨어가 프로세스로 실행되고 그 위에서 구동하는 식이였다.

요즘은 하드웨어 가상화를 통해 vm이 하드웨어와 직접 맞물려 있다.

## in deepth hardware virtualization

[하드웨어 가상화](https://en.wikipedia.org/wiki/Hardware-assisted_virtualization)는 cpu에 의해 기능이 제공되며 인텔 계열은 [Intel® VT](https://www.intel.com/content/www/us/en/virtualization/virtualization-technology/intel-virtualization-technology.htmt) 이고 AMD 계열은 AMD-V 라고 했다.  

cpu에서 제공되는 하드웨어 가상화를 os에서 소프트웨어로 구현한걸 `하이퍼바이저`라 

부르고 윈도우의 하이퍼바이저를 Hyper-v라고 부른다 했다.









































