###########环境依赖
CMake version > 3.5
Visual Studio Code
libtag_c.a, libtag.a
libqsqlmysql.so
在CMakeLists.txt 添加 find_package(Qt5 COMPONENTS Widgets Multimedia Sql REQUIRED)
TagLib下载地址：https://taglib.org/api/index.html

###########运行步骤
进入Music目录执行终端输入 mkidr build && mkidr bin

进入刚才创建好的 build 文件夹执行 cmake .. && make && ../bin/Music

###########V1.0.0 版本内容更新
新功能 完成基本布局
新功能 完成部分控件美化
新功能 利用TagLib获取歌曲的Tag信息

###########V1.1.0 版本内容更新
新功能 添加MySQL数据库，收藏列表保存到 MySQL
新功能 刷播放列表

###########V1.1.1 版本内容更新
新功能 添加对网易云音乐的支持
新功能 添加播放网络歌曲
新功能 获取网络歌曲的专辑，图片等

