# My Environment

- 操作系统：Windows 10
- Qt版本：5.12.8

## 1. Build & Install

这是一个基于CMake构建的工程，你可以在你自己的CMake工程里面添加它，或者是单独编译之后再将输出文件添加到自己的工程里面。

如果使用QtCreator，那么是非常方便的，你可以直接在QtCreator打开该工程的CMakeLists.txt文件，然后执行构建命令（Ctrl+B）编译工程。

编译成功之后，你可以在*项目*页面里面执行下面两部操作，完成执行文件的安装部署：

1. 在*CMake*页里面修改`CMAKE_INSTALL_PREFIX`为自己希望的安装路径，并点击`Apply Configuration Changes`；
2. 在*Build的步骤*页里面选择`targets`为`install`；
3. 执行构建命令（Ctrl+B）。

此时，在安装路径下将会出现一个install文件夹，install文件夹内也包含一个GraphicsMapLibd.dll文件。

如果你使用CMake工程来包含这个库，那上面的办法确实很方便，可以在每次编译完之后部署最新的dll，同时调试源码也非常方便。不过如果你使用非CMake工程来使用这个库，那么请手动拷贝.h文件、lib文件、dll文件。

## 2. Quick Start

1. 显示一个地图：
   ```
      auto map = new GraphicsMap;
      map->setTilePath("E:/map/sate");
      map->show();
   ```
   ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_sate.png)

2. 隐藏滚动条：
   
   ```
       map->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
       map->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
   ```
   ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_hide_scroll.png)

3. 设置缩放等级：

   ```
       map->setZoomLevel(5);
   ```
   ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_zoom.png)

4. 切换瓦片资源类型：

   ```
      map->setTilePath("E:/map/road");
   ```
   ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_road.png)

5. 设置鼠标中心缩放和鼠标拖动地图：

   ```
       map->setDragMode(QGraphicsView::ScrollHandDrag);
       map->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
   ```

6. 创建鼠标可交互地图：

   ```
      auto map = new InteractiveMap;
      map->setTilePath("E:/map/sate");
      map->show();
   ```

7. 为可交互地图绑定地图操作器

   - 圆形操作器：`map->setOperator(new MapPolygonOperator);`
        ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_ellipse.png)
   - 多边形操作器：`map->setOperator(new MapPolygonOperator);`
        ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_polygon.png)
   - 图标操作器：`map->setOperator(new MapObjectOperator);`
        ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_obj.png)
   - 路线操作器： `map->setOperator(new MapRouteOperator);`
        ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_route.png)
   - 测距操作器：`map->setOperator(new MapRangeLineOperator);`
        ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_range.png)
   
8. 手动添加地图元素： `map->scene()->addItem()`

   ```
       auto range = new MapRangeRingItem;	// 距离环
       range->setCoordinate({0,30});
       range->setRadius(90);
       map->scene()->addItem(range);
       auto ellipse = new MapEllipseItem;	// 圆
       ellipse->setCenter({1.0, 30});
       ellipse->setSize({20e3, 20e3});
       ellipse->setBrush(Qt::green);
       map->scene()->addItem(ellipse);
   ```

   ![](https://raw.githubusercontent.com/Mud-Player/MudPic/main/02GraphicsMapLib/quick_custom.png)

## 3. Class List

### 3.1 Map

1. GraphicsMap：封装基础的地图加载算法
2. InteractiveMap：封装地图交互的框架

### 3.2 Map Items

1. MapEllipseItem：圆形图形/椭圆图形
2. MapLabelItem：文本标签，由标题和内容组成
3. MapLineItem：线段
4. MapObjectItem：图标对象
5.  MapPieItem：扇形，由三角形和梯形组成
6. MapPolygonItem：多边形
7. MapRangeRingItem：距离环
8. MapRouteItem：航路
9. MapTrailItem：轨迹线

### 3.3 Map Operators

1. InteractiveMapOperator：地图操作器基类，定义处理交互事件的接口
2. MapEllipseOperator：圆形操作器
3. MapPolygonOperator：多边形操作器
4. MapObjectOperator：图标对象操作器
5. MapRouteOperator：航路操作器
6. MapRangeLineOperator：测距操作器

## 4. Bugs

1. OpenGL窗口下，MapObjectItem::setIconColor的第二个对象开始渲染会变成黑色方块

