# 2. Output an image
## 2-1. the ppm image format
There are some things to note in this code:

1. pixel은 차례대로 row로 표현된다.
2. 모든 pixel의 row는 1)왼쪽->오른쪽, 2) 위->아래의 순서로 표기된다.
3. RGB는 내부적으로 0.0~1.0의 실제 값변수로 표시된다.
   - 따라서 print 이전에 0~255 정수값으로 조정하는 과정이 필요하다
4. Red goes from fully off (black) to fully on (bright red) from left to right, and green goes from fully off at the top to black at the bottom. Adding red and green light together make yellow so we should expect the bottom right corner to be yellow.

## 2-2. creating an image file

# 3. The vec3 Class
거의 모든 그래픽 프로그램에는 기하학적 벡터와 색상을 저장하기 위한 클래스가 있습니다. 많은 시스템에서 이 벡터는 4D(3D 위치 + 기하학에서는 균질 좌표 또는 RGB + 색상에서는 알파 투명 구성 요소)입니다. 우리의 목적을 위해서는 세 개의 좌표로 충분합니다. 우리는 색상, 위치, 방향, 오프셋 등에 동일한 클래스 vec3를 사용할 것입니다.

여기서는 double을 사용하지만 일부 광선 추적기는 float를 사용합니다. double은 정확도와 범위가 더 높지만 float에 비해 크기가 두 배입니다. 이 크기 증가는 제한된 메모리 조건(하드웨어 셰이더 등)에서 프로그래밍하는 경우 중요할 수 있습니다. 둘 중 하나라도 괜찮습니다. 취향에 따라 하십시오

# 4. Rays, a Simple Camera, and Background
## 4.1 the ray class
모든 ray-tracer가 갖는 유일한 것은 ray class와 ray를 따라 보이는 색에 대한 computation이다.

Let’s think of a ray as a function 𝐏(𝑡)=𝐀+𝑡𝐛. 𝐏는 선을 따라 있는 3D 상의 위치다. 𝐀는 ray 원점이고, 𝐛는 ray direction이다.

ray prameter 𝑡는 상수(코드 상에서는 double)이며, t에 따라 𝐏(𝑡)이 ray를 따라 이동한다. 
ray에서는 양의 t만을 취급하고, 이때 P(t) 성분공간을 half-line이라고도 한다.
(Add in negative 𝑡 values and you can go anywhere on the 3D line. For positive 𝑡, you get only the parts in front of 𝐀, and this is what is often called a half-line or a ray.)

We can represent the idea of a ray as a class, and represent the function 𝐏(𝑡) as a function that we'll call `ray::at(t):`

## 4.2 Sending Rays Into the Scene

ray tracer는 픽셀들을 통해 ray들을 보내고, ray들의 direction에서 보이는 color를 계산한다. 이 과정의 단계는 다음과 같다;

1. Calculate the ray from the “eye” through the pixel,
2. Determine which objects the ray intersects, and
3. Compute a color for the closest intersection point.

aspect ratio -> img_h, img_w   
For a practical example, an image 800 pixels wide by 400 pixels high has a 2∶1 aspect ratio.

apect ratio가 주어진다고 가정하고, img_w를 미리 설정한후 비율에 맞춰 img_h을 계산하는것 권장
This way, we can scale up or down the image by changing the image width. 
이떄 적어도 img_h은 1이상이 되도록해야 ㅎ나다. 

In addition to setting up the pixel dimensions for the rendered image, we also need to set up a virtual viewport through which to pass our scene rays
viewport: 이미지 픽셀 위치의 그리드를 포함하는 3D상의 가상 직사강형이다.
- If pixels are spaced the same distance horizontally as they are vertically, the viewport that bounds them will have the same aspect ratio as the rendered image. 
- 인접한 두 픽셀 간의 거리를 pixel spacing이라 하고, square pixel이 표준이다.
- To start things off, we'll choose an arbitrary viewport height of 2.0, and scale the viewport width to give us the desired aspect ratio.

```
```
aspect ratio를 viewport_w를 계산할 시 사용하지 않는 이유: because the value set to aspect_ratio is the ideal ratio, it may not be the actual ratio between image_width and image_height.
- image_h이 real value를 따른다면, aspect_ratio를 사용해도 된다.
- 그러나 실제 코드에선,
  1. (오타발견!) image_h은 가장 가까운 정수로 반올림되므로 비율을 증가시킬 수 있다.
  2. 실제 종횡비를 변경할 수 있는 image_h는 1보다 작도록 허용되지 않는다.

| Note that aspect_ratio is an ideal ratio, which we approximate as best as possible with the integer-based ratio of image width over image height.

viewport 비율이 image 비율과 정확히 일치하도록 하기 위해서, image aspect ratio를 사용하여 최종 viewport width를 정한다.

다음으로, camera center를 정한다;
모든 ray가 발생하는 3D 공간의 한점(일반적으로, eye point라고도 한다)

- camera center에서 viewport center까지의 벡터는 viewport와 직교한다.
- viewport와 camera center 사이의 거리를 one unit으로 지정한다. 이 거리를 흔히 focal length라고 한다.


카메라 센터를 단순하게 (0,0,0)으로 설정하여 시작하겠다.   
y축은 위로 올라가고, x축은 오른쪽으로 가고, 음의 z축은 보는 방향으로 향한다.
(This is commonly referred to as right-handed coordinates.)

[이미지]

- 왼쪽 위에 0번째 픽셀을 두고 오른쪽 아래에 있는 마지막 픽셀까지 작업하려는 이미지 좌표와 충돌한다.
  - 이는 이미지 좌표 Y축이 반전되었음을 의미한다. y축은 이미지 아래로 갈수록 증가한다.
- 이미지를 스캔할 때 왼쪽 상단 픽셀(pixel 0,0)에서 시작하여 각행에서 왼쪽->오른쪽(𝐕𝐮)으로 스캔한 후 다음 행으로 위->아래(𝐕𝐯)로 가며 스캔한다.
- pixel grid는 viewport edges에 픽셀거리 절반만큼 삽입된다.
- 이러한 방식으로, viewport 영역은 wxh가 동일한 영역으로 고르게 분할된다.
뷰포트 및 픽셀 그리드의 모양은 다음과 같습니다:

[이미지]

위 이미지에서, 우리는 7x5 해상도 이미지에 대한 viewport와 pixel grid를 가진다. 
- the viewport upper left corner 𝐐, the pixel 𝐏0,0 location
- the viewport vector 𝐕𝐮 (viewport_u), the viewport vector 𝐕𝐯 (viewport_v)
- the pixel delta vectors 𝚫𝐮 and 𝚫𝐯.

Drawing from all of this, here's the code that implements the camera. We'll stub in a function ray_color(const ray& r) that returns the color for a given scene ray — which we'll set to always return black for now.  

```
```
- 위 코드에서 ray_direction을 unit vector로 바꿔쓰지 않았다. 간단한 ray function에서는 unit vector가 아니여도 되기 때문이다.

이제 `ray_color(ray)` 함수를 간단한 기울기 함수로 수정하여 위의 color(0,0,0)로 채워진 백지를 색으로 채울것이다. 
This function will linearly blend white and blue depending on the height of the 𝑦 coordinate(unit length로 스케일링되어 있는 y좌표, -1.0<y<1.0 ) 
- I'll use a standard graphics trick to linearly scale 0.0≤𝑎≤1.0. When 𝑎=1.0, I want blue. When 𝑎=0.0, I want white.
- This forms a “linear blend”, or “linear interpolation”. This is commonly referred to as a lerp between two values. A lerp is always of the form; blendedValue=(1−𝑎)⋅startValue+𝑎⋅endValue,

# 5. Adding a Sphere
Let's add a single object to our ray tracer.
구를 넣어볼거에오!

## 5.1 Ray-Sphere Intersection
반지름 r 구 방정식 = $𝑥^2+𝑦^2+𝑧^2=𝑟^2$
- if a given point (𝑥,𝑦,𝑧) is on the sphere, then 𝑥2+𝑦2+𝑧2=𝑟2. 
- If a given point (𝑥,𝑦,𝑧) is inside the sphere, then 𝑥2+𝑦2+𝑧2<𝑟2, 
- and if a given point (𝑥,𝑦,𝑧) is outside the sphere, then 𝑥2+𝑦2+𝑧2>𝑟2.

구 원점(𝐶𝑥,𝐶𝑦,𝐶𝑧)이 주어졌다면 ; $(𝑥−𝐶𝑥)^2+(𝑦−𝐶𝑦)^2+(𝑧−𝐶𝑧)^2=𝑟^2$
vector로 표현하게 되면 𝐂=(𝐶𝑥,𝐶𝑦,𝐶𝑧) to point 𝐏=(𝑥,𝑦,𝑧) is (𝐏−𝐂) vector로 식을 쓸수 있다.; $(𝐏−𝐂)⋅(𝐏−𝐂)=(𝑥−𝐶𝑥)^2+(𝑦−𝐶𝑦)^2+(𝑧−𝐶𝑧)^2=r^2$

We want to know if our ray 𝐏(𝑡)=𝐀+𝑡𝐛 ever hits the sphere anywhere
ray와 구의 교점 P(t)를 만족시키는 t에 대해 식을 쓰면 다음과 같다; 
$(𝐏(𝑡)−𝐂)⋅(𝐏(𝑡)−𝐂)=𝑟^2$
$((𝐀+𝑡𝐛)−𝐂)⋅((𝐀+𝑡𝐛)−𝐂)=𝑟^2$
$(𝑡𝐛+(𝐀−𝐂))⋅(𝑡𝐛+(𝐀−𝐂))=𝑟^2$
$𝑡^2𝐛⋅𝐛+2𝑡𝐛⋅(𝐀−𝐂)+(𝐀−𝐂)⋅(𝐀−𝐂)=𝑟^2$
$𝑡^2𝐛⋅𝐛+2𝑡𝐛⋅(𝐀−𝐂)+(𝐀−𝐂)⋅(𝐀−𝐂)−𝑟^2=0$

t에 관해서 식을 풀어내면
$\frac{-(2𝐛⋅(𝐀−𝐂)) \pm \sqrt{(2𝐛⋅(𝐀−𝐂))^2 - 4(𝐛⋅𝐛)((𝐀−𝐂)⋅(𝐀−𝐂)−𝑟2)}}{2(𝐛⋅𝐛)}$
$\frac{-b \pm \sqrt{b^2 - 4ac}}{2a}$
- 루트값이 양수이라면: 2 real solution
- 루트값이 음수라면: no soultion
- 루트값이 0이라면: 1 real solution

[이미지]

## 5.2 Creating Our First Raytraced Image
If we take that math and hard-code it into our program, we can test our code by placing a small sphere at **−1 on the z-axis** and then coloring red any pixel that **intersects** it.

```
```
- 주의할점은 구 center 값이 (0,0,+1)이어도 똑같이 그림을 그려낸다는 것이다.
- 이는 아직 카메라가 카메라 앞의 물체와 뒤의 물체를 구분하지 않기 때문에 이와같이 나타난다.

# 6. Surface Normals and Multiple Object
## 6.1 shading with surface normals
그림자 만들거다. 그럴려면 surface normal을 구해야한다.
surface normal: 표면에 수직인 벡터다.

normal vector가 1)임의의 길이를 가질지, 2)단위길이로 정규화할지 선택하자.
- normalization을 위한 비싼 루트 연산은 생략하고 싶겠지만 실제 3가지 주요 관측에 맞춰 선택하는 것이 좋다
  
1. 만약 unit-length normal vector가 필요하다면, 모든 위치에 대해 normalization을 해주는 것이 좋다
2. we do require unit-length normal vectors in several places
3.  if you require normal vectors to be unit length, then you can often efficiently generate that vector with an understanding of the specific geometry class, in its constructor, or in the hit() function
    - ex) 구면 법선은 단순히 구 반지름으로 나눔으로써 단위 길이를 만들 수 있으며, 제곱근을 완전히 피할 수 있다.

결과적으로, normal vector가 unit length인 정책을 채택한다.

구의 경우, 바깥쪽 법선은 적중점에서 중심을 뺀 방향이다.
[이미지]

- 색에 대 정규분포를 적용해서 시각화 할 것이다.
  - -1에서 1사이인 unit length vector를 0~1사이의 간격을 가지는 각 성분에 매핑한 다음 (x,y,z)를 RGB로 매핑하는 것이다.

앞서 교차점이 존재하는 지 여부만 확인했다면, 이제는 교차점이 어딘지도 알아야한다.
우리는 하나의 구만을 scene에서 가지고 있고, 카메라 바로 앞에 있으므로 음수 t에 대해서는 생각하지 않아도 된다. 우리는 가장 가까운 교차점 t에 대해서만 연산하고 시각화할 것이다.


## 6.2 Simplifying the Ray-Sphere Intersection Code

1. vector 자기 자신 내적은 루트 해당 vector의 길이와 같다
2. b=2h인 경우 식이 간략해진다.(b= 2b(A-C))

따라서 hit_sphere를 다음과 같이 쓸 수 있다.

```
```

## 6.3 An Abstraction for Hittable Objects
구 여러개
“abstract class”를 만들어 hit array list를 만드는것

What that class should be called is something of a quandary — calling it an “object” would be good if not for “object oriented” programming. “Surface” is often used, with the weakness being maybe we will want volumes (fog, clouds, stuff like that). “hittable” emphasizes the member function that unites them. I don’t love any of these, but we'll go with “hittable”.

“hittable” 추상 클래스에는 ray을 가지는 hit function이 들어간다.
- 대부분의 ray tracer는 t_min, t_max 값을 통해, t_min<t<t_max인 경우 hit되었다고 count한다.
  
## 6.4 Front Faces Versus Back Faces 
normal vector는 항상 center에서 intersection point로의 방향을 가진다.   
- 구 밖에서 들어온 ray가 intersection 되는 경우, normal은 ray와 반대로 위치한다.
- 반대로, 구 안에서 ray를 쏜 경우, normal은 ray와 동일하게 위치한다.

대신, 우리는 normal이 항상 ray에 반대하도록 할 수 있다.
- 광선이 구 밖에 있다면, normal은 밖을 가리키지만
- ray가 구 안에 있다면, normal은 안을 가리킬 것이다.
[이미지]

ray가 어느 방향에서 오는지 정해야하기 때문에 우린 둘중하나를 골라야한다.
If the ray and the normal face in the same direction, the ray is inside the object
if the ray and the normal face in the opposite direction, then the ray is outside the object.

이것은 ray와 normal의 내적값에 따라 알 수 있다; 양수면, inside the object다.

If we decide to have the normals always point against the ray, we won't be able to use the dot product to determine which side of the surface the ray is on. Instead, we would need to store that information:

## 6.5 A List of Hittable Objects

## 6.6 Some New C++ Features 
`shared_ptr<type>`: pointer to some allocated type이다. reference-counting semantics를 가진다.
- 다른 공유 포인터에 값을 할당할때마다 reference count는 증가된다.
- reference count가 0이 되면 안전하게 object는 삭제된다.

Typically, a shared pointer is first initialized with a newly-allocated object, something like this:
```
shared_ptr<double> double_ptr = make_shared<double>(0.37);
shared_ptr<vec3>   vec3_ptr   = make_shared<vec3>(1.414214, 2.718281, 1.618034);
shared_ptr<sphere> sphere_ptr = make_shared<sphere>(point3(0,0,0), 1.0);
```
- make_shared<thing>(thing_constructor_params ...) allocates a new instance of type thing, using the constructor parameters. It returns a shared_ptr<thing>.
- auto 사용가능
- <memory> header

We'll use shared pointers in our code, because it allows multiple geometries to share a common instance (for example, a bunch of spheres that all use the same color material), and because it makes memory management automatic and easier to reason about.

# 7. Antialiasing
실제 카메라에서는 가장자리 픽셀은 일부 foreground와 background의 혼합이기 때문에 일반적으로 지글재글하게 보이지 않는다.
각 픽셀 내부의 많은 샘플을 평균화함으로써 동일한 효과를 얻을 수 있다.
stratification는 신경쓰지 않을 것이다.
## 7.1 some random numer utilities
real random number를 생성하는 난수생성기가 필요하다. 우리는 0≤𝑟<1 범위의 실수를 반환하는 함수가 필요하다

| A simple approach to this is to use the rand() function that can be found in <cstdlib>. This function returns a random integer in the range 0 and RAND_MAX. 

```
```

<random> header를 통해서도 난수생성기를 만들수 있다
```
```

## 7.2 generating pixels with multiple samples
주어진 픽셀에 대해 우리는 그 픽셀 안에 여러 개의 샘플을 가지고 있고 각각의 샘플을 통해 광선을 보냅니다. 그리고 나서 이 광선의 색상은 평균이 됩니다:
[이미지]

지금이 우리의 가상 카메라와 scene scampling의 관련 작업을 관리하기 위한 카메라 클래스를 만들 때입니다. 다음 클래스는 이전의 축 정렬 카메라를 사용하여 간단한 카메라를 구현합니다:

```
```

다중 샘플링된 색상 계산을 처리하기 위해 write_color() 함수를 업데이트합니다
Rather than adding in a fractional contribution each time we accumulate more light to the color, just add the full color each iteration, and then perform a single divide at the end (by the number of samples) when writing out the color.

rtweekend.h utility header를 추가한다: clamp(x,min,max), which clamps the value x to the range [min,max]:

# 8. Diffuse Materials
무광 재질을 만들것이다.
One question is whether we mix and match geometry and materials or
if geometry and material are tightly bound

## 8.1 A Simple Diffuse Material
diffuse object는 주변 색을 띠지만, 고유 색으로 변조된다.
확산된 표면에서 반사되는 빛은 방향이 무작위로 결정된다.   
따라서 3개의 ray를 2개의 diffuse surface로 보내면, 각각 무작위로 행동한다.
[이미지]

또한, 위처럼 반사될수도 반면 흡수될 수도 있다.
- 표면이 어두울수록 ray가 흡수될 간으성이 더 높다.
  
랜덤 direction algorithm으로 무광 표면을 만들어낼 수 있다. 
=> Lambertian
표면의 hit point에 접하는 두개의 단위 반지름 구가 있다
- 이 두 구는 p+n, p-n center를 가진다.
- 중심이 p+n인 구는 hit surface 안쪽이고, p-n은 hit surface 밖이다.
- ray origin과 같은 방면에 있는 접선 단위 반지름 구를 선택한다
- 이 단위 반지름 구 안의 임의의 점 S를 선택하고 hit point P에서 S로 ray를 보낸다(S-P)
[이미지]

We need a way to pick a random point in a unit radius sphere. We’ll use what is usually the easiest algorithm: a rejection method
먼저 x, y, z의 범위가 모두 -1에서 +1인 임의의 점을 선택합니다. 이 점을 거부하고 점이 구 밖에 있으면 다시 시도하십시오.

## 8.2 Limiting the Number of Child Rays
ray_color가 재귀함수다.
어떤것에도 부딪히지 못할때 재귀를 멈춘다. 긴시간 소요되는 것을 방지하기 위해 최대 재귀 깊이를 제한하여 최대 깊이에서 ray_color를 반환하지 않도록한다.

## 8.3 Using Gamma Correction for Accurate Color Intensity
구 밑 shadowing.
위의 코드 결과는 너무 어둡다. 그러나, 구는 오직 각 bounce에 대해 절반의 에너지만을 흡수한다. 따라서 50%는 반사되는 것이다.
만약 그림자가 잘 안보여도 걱정말아라.
그 이유는, 대부분의 이미지 viewers는 image가 "gamma corrected"되었다고 추정하다. 
- meaning the 0 to 1 values have some transform before being stored as a byte.

따라서 우리는 color에 1/gamma를 거듭 제곱하는 "gamma 2"를 적용할 것이다. 
- 가장 간단한 경우, 1/2이다. 즉 루트

## 8.4 Fixing Shadow Acne
여기에는 bug가 있다.
몇 반사된 ray는 정확히 t=0이 아닌 object를 hit한다. 구 intersector에 근사하는 𝑡=−0.0000001 or 𝑡=0.00000001지점을 말이다.

따라서 우리는 0 근사를 hit하는 것을 막아야 한다.
```
if (world.hit(r, 0.001, infinity, rec)) {
```

## 8.5 True Lambertian Reflection
rejection method는 unit ball offset 안에 suface normal 방향을 따르는 random point를 만든다.
- 이것은 normal에 가까울 높은 확률과 grazing 각도에서 ray을 scattering할 확률이 낮은 반구의 방향을 선택하는것이다.
- 분산은 cos3(𝜙)에 의해 scale된다. 𝜙: noraml의 각
- 이것은 얕은 각도로 도달하는 빛이 더 넓은 영역에 걸쳐 퍼지기 때문에 유용하며, 따라서 얕은 각도에 대한 ray는 최종 색상에 대한 기여도가 낮습니다.

However, we are interested in a Lambertian distribution, which has a distribution of cos(𝜙)
- rejection 방식과는 다르게 Lambertian은 normal에 가까운 ray scattering의 확률이 더 높지만 분포는 더 균일합니다
- unit sphere의 suface point를 고르고 surface normal을 따라 오프셋을 설정한다
- 단위 구에서 임의의 점을 선택하는 것은 임의의 점을 선택(in the unit sphere)한 다음 정규화함으로써 가능하다.

It's hard to tell the difference between these two diffuse methods, given that our scene of two spheres is so simple, but you should be able to notice two important visual differences:

1. 변경 후 그림자가 덜 두드러진다. The shadows are less pronounced after the change
2. 변경 후 두 구 모두 밝아졌다. Both spheres are lighter in appearance after the change

이 두 가지 변화 모두 더 균일한 ray의 scattering으로 인해 normal 방향으로 더 적은 광선이 산란하게 됩니다. 이것은 diffuse되는 물체의 경우 더 많은 빛이 카메라를 향해 튕겨 나오기 때문에 더 밝게 나타난다는 것을 의미합니다. 그림자의 경우 더 적은 빛이 똑바로 위로 튀어오르기 때문에 더 작은 구 바로 아래에 있는 더 큰 구의 부분이 더 밝습니다.

## 8.6 An Alternative Diffuse Formulation
Lambertian diffuse에 대한 부정확한 근사치로 인해 코드에 hack이 있다. error의 큰 이유는 다음이 어려웠기 때문이다;
1. 확률분포가 틀린것을 수학적으로 증명
2. cos 분포가 바람직한 이유 직관적으로 설명

흔하지 않지만, 일상적인 물체는 완벽하게 확산되어 있기 때문에 이러한 물체가 빛 아래에서 어떻게 행동하는지에 대한 시각적 직관이 제대로 형성되지 않을 수 있습니다.

더 직관적인 접근법은 normal 각도에 의존하지 않고 히트 포인트에서 떨어진 모든 각도에 대해 균일한 산란 방향을 갖는 것입니다

# 9. Metal
## 9.1 An Abstract Class for Materials
만약 우리가 다른 object가 다른 material을 갖길 원한다면, design decision이 필요하다.
we could have an abstract material class that encapsulates behavior. 

For our program the material needs to do two things:
1. scatterd ray를 생성한다.
2. 만약 scattered되었다면, ray를 얼마나 감쇄해야하는지 말해준다.

## 9.2 A Data Structure to Describe Ray-Object Intersections
The hit_record is to avoid a bunch of arguments so we can stuff whatever info we want in there. 

hittable과 material은 서로 상호 reference되어 있어야한다.
```
```
여기서 설정한 것은 우리가 surface와 어떻게 상호작용하는지 말해 줄 것이다.
-  When a ray hits a surface (a particular sphere for example), the material pointer in the hit_record will be set to point at the material pointer the sphere was given when it was set up in main() when we start.
-  When the ray_color() routine gets the hit_record it can call member functions of the material pointer to find out what ray, if any, is scattered.

## 9.3 Modeling Light Scatter and Reflectance
램버시안(확산)에 대해서, 그것은 
1)항상 산란하고 반사율 R에 의해 감쇠될 수 있거나, 
2)감쇠 없이 산란할 수 있지만 ray의 1/1-R을 흡수할 수 있거나, 
3) 또는 그러한 전략의 혼합일 수 있습니다. 

램버시안 물질의 경우, 우리는 다음과 같은 간단한 class을 얻습니다:

우리는 단지 약간의 확률 𝑝로 흩어지고 albedo/p로 감쇄되다는 것을 유의하십시오.

위의 코드를 주의 깊게 읽으면 작은 장난 가능성을 알게 될 것입니다. 우리가 생성하는 임의의 단위 벡터가 정규 벡터와 정확히 반대면 둘은 0으로 합해져서 0 scatter 방향 벡터가 됩니다. 이는 나중에 나쁜 시나리오(무한과 NaNs)로 이어지므로 전달하기 전에 조건을 가로채야 합니다.

이를 위해 벡터가 모든 차원에서 0에 매우 가까운 경우 true를 반환하는 새로운 벡터 방법인 vec3::near_zero()를 생성합니다.

```
if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
```
> false인 경우에는??? 그냥 normal???

## 9.4 Mirrored Light Reflection
For smooth metals the ray won’t be randomly scattered. 
The key math is: how does a ray get reflected from a metal mirror? Vector math is our friend here:
[이미지]
반사된 ray direction은 v+2b. //?????왜???
우리 설계에서 n은 unit vector이고, v는 아니다. 
b의 length는 vdotn이여야만 한다.

## 9.5 A Scene with Metal Spheres
## 9.6 Fuzzy Reflection
We can also randomize the reflected direction by using a small sphere and choosing a new endpoint for the ray:

구가 클수록, fuzzier(불투명?) 반사가 된다.

fuzziness parameter를 추가한다. 
- 문제는 큰 구들이나 grazing ray의 경우, 우리는 표면 아래로 scatter 될수 있는 것입니다. 우리는 표면이 그것들을 흡수하도록 할 수 있습니다.

# 10. Dielectrics
물,유리와 같은 clear material을 dielectrics라 한다.
light ray가 부딪히면, 해당 ray는 reflected ray와 refracted ray로 나뉜다.

우리는 이것을 reflection과 refraction 중에 하날 random하게 골라, 하나의 scattered ray per interaction을 생성할 것이다.

## 10.1 Refraction
The hardest part to debug is the refracted ray
I usually first just have all the light refract if there is a refraction ray at all. For this project, I tried to put two glass balls in our scene.

## 10.2 Snell's law
The refraction is described by Snell’s law:
$\eta \cdot \sin\theta = \eta' \cdot \sin\theta'$

- Where 𝜃 and 𝜃′ are the angles from the normal
- and 𝜂 and 𝜂′ (pronounced “eta” and “eta prime”) are the refractive indices 
  - (typically air = 1.0, glass = 1.3–1.7, diamond = 2.4)

[이미지]

굴절 ray의 direction을 결정하기 위해선 우리는 sin{theta}에 대해서 해결해랴한다.
$\sin\theta' = \frac{\eta}{\eta'} \cdot \sin\theta$

표면에서의 굴절에서 굴절 ray을 R'라하고, normal을 n'이라한다. 그 사이 vector간 angle을 theta'이다.
우리는 R'을 n'에 수직인 ray와 n'에 평행한 ray로 나눌 수 있다.

$\mathbf{R'} = \mathbf{R'}_{\bot} + \mathbf{R'}_{\parallel}$
$\mathbf{R'}_{\bot} = \frac{\eta}{\eta'} (\mathbf{R} + \cos\theta \mathbf{n})$
$\mathbf{R'}_{\parallel} = -\sqrt{1 - |\mathbf{R'}_{\bot}|^2} \mathbf{n}$

We still need to solve for cos𝜃.

$\mathbf{a} \cdot \mathbf{b} = |\mathbf{a}| |\mathbf{b}| \cos\theta$

위의 식을 사용해서 풀어낼수 있다. a,b가 unit vector인 경우 벡터 절대값은 상쇄된다.

$\mathbf{R'}_{\bot} =
     \frac{\eta}{\eta'} (\mathbf{R} + (\mathbf{-R} \cdot \mathbf{n}) \mathbf{n})$

따라서 코드 상에서 R'을 연산하면 다음과 같다.


## 10.3 Total Internal Reflection

10.2의 코드 결과는 잘못된것 처럼 보인다.
그 이유의 첫번째 문제는 굴절률이 높은 물질에서의 ray에 관해 snell'law는 real solution을 가지지 못하기 때문이다.
따라서 굴절이 불가능하다

snell's law에 대해 sin의 관점에서 보자면;
$\sin\theta' = \frac{\eta}{\eta'} \cdot \sin\theta$

If the ray is inside glass and outside is air (𝜂=1.5 and 𝜂′=1.0):
$\sin\theta' = \frac{1.5}{1.0} \cdot \sin\theta$

따라서 sin{theta'}는 1보다 클수 없다. 그러므로 $\frac{1.5}{1.0} \cdot \sin\theta > 1.0$와 같은 경우 해가 존재할 수 없다.
**해가 존재하지 않으면 유리는 굴절될 수 없으므로 광선을 반사해야한다.**

이경우 모든 빛이 반사되며, 일반적으로 고체 물체 내부에 있기 때문에 "total internal reflection"라고 부른다.
이것이 물과 공기의 경계가 완벽한 거울처럼 작용하는 이유다.
sin_theta에 대해 삼각법적 특성을 사용하여 해결할 수 있다;

$\sin\theta  = \sqrt{1 - \cos^2\theta}$

$\cos\theta = \mathbf{R} \cdot \mathbf{n}$

이므로

```
```

해당 결과에서는 유리가 반사하지는 않는다.

## 10.4 Schlick Approximation
이제 진짜 유리는 각도에 따라 다른 반사율을 가지고 있습니다. 
- 창문을 가파른 각도로 바라보면 거울이 됩니다. 
Christophe Schlick의 값싸고 놀랍게도 정확한 다항식 근사를 사용해 구현할 수 있다.
이것은 우리의 완전한 유리 재료를 산출합니다:

## 10.5 Modeling a Hollow Glass Sphere
dielectric 구에 대한 흥미롭고 쉬운 트릭은 음의 반지름을 사용하면 기하학에 영향을 주지 않지만 표면은 정상적으로 안쪽을 가리킨다는 것에 주목하는 것입니다. 
이것은 속이 빈 유리 구를 만드는 기포로 사용될 수 있습니다:

# 11. Positionable Camera
카메라는 dielectrics와 마찬가지로 디버깅하기가 어렵다. 
1. 조정가능한 field of view를 허용한다.
   - portal을 통해 보는 각도다.
   - 우리의 이미지가 정사각형이 아니기 때문에 fov는 수평과 수직이 다르다.
   - 항상 수직 fov를 사용한다.
   - 보통 각도를 지정하고 constructor 내부의 radian으로 변경한다.
## 11.1 Camera Viewing Geometry
origin에서 부터 z=-1 Plane으로 향하는 ray에 대해서
우리는 거리에 대한 비율 h을 만들기만 하면 z=-2 plane 또는 무엇이든 만들 수 있다.
[이미지]
이것은 h=tan(theta/2)를 적용하면된다.

## 11.2 Positioning and Orienting the Camera
임의의 관점을 얻기 위해, 먼저 우리가 관심을 가지는 지점의 이름을 붙이도록 하겠습니다. 
우리는 카메라를 바라보는 위치(lookfrom)와 바라보는 지점(lookat)을 부를 것입니다. 
(나중에 원한다면 볼 지점 대신 볼 방향을 정의할 수 있습니다.)

우리는 또한 카메라의 롤(roll), 즉 축에서 바라보는 시선(lookat-lookfrom)의 회전을 지정하는 방법이 필요합니다. 
이에 대해 생각할 수 있는 또 다른 방법은 시선을 유지하고 일정하게 주시하더라도 여전히 nose 주변에서 head를 회전시킬 수 있다는 것입니다. 
우리가 필요한 것은 카메라를 위한 "up" 벡터를 지정하는 방법입니다. 
이 up 벡터는 view direction과 직교하는 평면에 있어야 합니다.

[이미지]

우리는 실제로 우리가 원하는 임의의 up 벡터를 사용할 수 있고, 
카메라의 up 벡터를 얻기 위해 단순히 이 평면에 투영할 수 있습니다. 
나는 "view up" (vup) 벡터의 이름을 짓는 일반적인 관례를 사용합니다. 
두 개의 교차 제품이 있으며, 이제 우리는 카메라의 방향을 설명하기 위한 완전한 orthonormal basis (𝑢,𝑣,𝑤)을 갖게 되었습니다.

vup, v, w가 모두 같은 평면에 있다는 것을 기억하십시오. 
이전에 카메라가 -Z를 향하도록 고정했을 때처럼 임의의 뷰 카메라가 -w를 향한다는 것을 기억하십시오. 
그리고 vup을 지정하기 위해 world up (0,1,0)을 사용할 수 있지만 그럴 필요는 없습니다. 
이것은 편리하며 미친 카메라 각도로 실험하기로 결정할 때까지 카메라를 자연스럽게 수평으로 유지합니다.

# 12. Defocus Blur
마지막 기능: defocus blur. 
일반적으로 "depth of field"라고 부르고 
여기서만 "defocus blur"라고 할것이다. 

우리가 실제 카메라에서 blur에 초점을 맞추는 이유는 
카메라가 빛을 모으기 위해 (단순히 핀홀이 아닌) 큰 구멍이 필요하기 때문입니다. 
이것은 모든 것에 초점을 맞출 것이지만, 만약 우리가 그 구멍에 렌즈를 꽂는다면, 
모든 것이 초점을 맞추는 특정한 거리가 있을 것입니다. 
여러분은 이런 방식으로 렌즈를 생각할 수 있습니다: 

초점 거리의 특정한 지점에서 나오는 모든 ray(렌즈에 부딪히는)는 이미지 센서의 한 지점으로 다시 구부러질 것입니다.

우리는 projection point과 모든 것이 완벽하게 초점을 맞춘 평면 사이의 거리를 focus distance라고 부릅니다. 
focus distance는 focal lenght와 같지 않다는 것을 유의하십시오. 
초점 거리는 projection point과 image plane 사이의 거리입니다.

물리적 카메라에서는 focus distance가 lens와 film/sensor 사이의 거리에 의해 제어됩니다. 
그래서 초점이 맞는 것을 바꿀 때(휴대폰 카메라에서도 일어날 수 있지만 센서는 움직입니다) 렌즈가 카메라에 비해 움직이는 것을 볼 수 있습니다. 
"구멍"은 렌즈가 효과적으로 얼마나 큰지 조절하기 위한 구멍입니다. 
실제 카메라의 경우 더 많은 빛이 필요하면 구멍을 더 크게 만들고 더 많은 defocus blur가 발생합니다. 
가상 카메라의 경우 완벽한 센서를 가질 수 있고 더 많은 빛이 필요하지 않으므로 디포커스 블러를 원할 때만 구멍이 있습니다.

## 12.1 A Thin Lens Approximation
실제 카메라에는 복잡한 복합 렌즈가 있습니다. 
우리 코드의 경우 sensor, lens, aperture 순으로 시뮬레이션할 수 있습니다. 
그런 다음 광선을 어디로 보낼지 파악하고 이미지를 뒤집을 수 있습니다(이미지가 필름에 거꾸로 투영됨). 
그러나 그래픽 사용자는 일반적으로 얇은 렌즈 근사치를 사용합니다:

[이미지]

우리는 카메라 내부를 시뮬레이션할 필요가 없습니다. 
카메라 외부의 이미지를 렌더링하기 위해 그것은 불필요한 복잡성일 것입니다. 
대신, 나는 보통 렌즈에서 광선을 시작하여 그 평면의 모든 것이 완벽한 초점이 있는 포커스 평면(focus_dist away from the lens)으로 보냅니다.

## 12.2 Generating Sample Rays
일반적으로 모든 장면 광선은 시점으로부터의 시선에서 비롯됩니다. 
defocus blur를 달성하려면 lookfrom point로 향하는 disk 내부에서 무작위 scene ray을 생성합니다.
반경이 클수록 디포커스 블러가 더 좋아진다. 
원래 카메라에는 반경 0(흐림 없음)의 디포커스 디스크가 있으므로 모든 광선은 디스크 중심(시점)에서 발생했습니다.


# 14. Where Next?
1. Lights — You can do this explicitly, by sending shadow rays to lights, or it can be done implicitly by making some objects emit light, biasing scattered rays toward them, and then downweighting those rays to cancel out the bias. Both work. I am in the minority in favoring the latter approach.
2. Triangles — Most cool models are in triangle form. The model I/O is the worst and almost everybody tries to get somebody else’s code to do this.
3. Surface Textures — This lets you paste images on like wall paper. Pretty easy and a good thing to do.
4. Solid textures — Ken Perlin has his code online. Andrew Kensler has some very cool info at his blog.
5. Volumes and Media — Cool stuff and will challenge your software architecture. I favor making volumes have the hittable interface and probabilistically have intersections based on density. Your rendering code doesn’t even have to know it has volumes with that method.
6. Parallelism — Run 𝑁 copies of your code on 𝑁 cores with different random seeds. Average the 𝑁 runs. This averaging can also be done hierarchically where 𝑁/2 pairs can be averaged to get 𝑁/4 images, and pairs of those can be averaged. That method of parallelism should extend well into the thousands of cores with very little coding.
