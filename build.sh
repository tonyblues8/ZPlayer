#!/bin/bash
#set -x
shellpath=`pwd`
[[ -f ./mylocalvideo ]] && echo "Deleting mylocalvideo..." && rm ./mylocalvideo
[[ -f ./myvideo ]] && echo "Deleting myvideo..." && rm ./myvideo

make clean && make && [[ -f ./mylocalvideo ]] || { echo "编译失败"; exit 1; }

chmod a+x ./mylocalvideo
chmod a+x ./myvideo

echo "编译成功"
mv -f ./mylocalvideo /Applications/家庭视频.app/Contents/MacOS/
echo "mv -f ./mylocalvideo /Applications/家庭视频.app/Contents/MacOS/ oKKKKKK"

mv -f ./myvideo /Applications/家庭影视中心.app/Contents/MacOS/
echo "mv -f ./myvideo /Applications/家庭影视中心.app/Contents/MacOS/ oKKKKKK"

APP="/Applications/家庭视频.app"
APP2="/Applications/家庭影视中心.app"
#FRAMEWORKS_PATH="$APP/Contents/Frameworks"
FRAMEWORKS_PATH="$APP/Contents/MacOS"
FRAMEWORKS_PATH2="$APP2/Contents/MacOS"
EXECUTABLE_PATH="$APP/Contents/MacOS/mylocalvideo"
EXECUTABLE_PATH2="$APP2/Contents/MacOS/myvideo"

# 使用 otool 获取非系统依赖列表
echo "分析可执行文件:mylocalvideo的动态库依赖..."
#DEPENDENCIES=$(otool -L "$EXECUTABLE_PATH" | tail -n +2 | awk '/\// {print $1}' | grep -Ev '^/usr/lib|^/System/Library')
DEPENDENCIES=$(otool -L "$EXECUTABLE_PATH" | sed -n '2,$p' | awk '/\// {print $1}' | grep -Ev '^/usr/lib|^/System/Library')

if [ -z "$DEPENDENCIES" ]; then
    echo "mylocalvideo没有非系统库依赖。"
else
    echo "mylocalvideo非系统库依赖列表:"
    echo "$DEPENDENCIES"


    for DEP in $DEPENDENCIES; do
        LIB_BASENAME=$(basename "$DEP")
        TARGET_PATH="$FRAMEWORKS_PATH/$LIB_BASENAME"
        if [ ! -f "$TARGET_PATH" ]; then
            echo "复制: $DEP -> $TARGET_PATH"
            cp "$DEP" "$FRAMEWORKS_PATH/"
            echo "更新动态库路径:$DEP -> @executable_path"
            install_name_tool -change "$DEP" @executable_path/$LIB_BASENAME "$EXECUTABLE_PATH"
        fi
    done
    echo "mylocalvideo所有动态库处理完成"
fi
# 使用 otool 获取非系统依赖列表
echo "分析可执行文件:myvideo的动态库依赖..."
#DEPENDENCIES=$(otool -L "$EXECUTABLE_PATH2" | tail -n +2 | awk '/\// {print $1}' | grep -Ev '^/usr/lib|^/System/Library')
DEPENDENCIES=$(otool -L "$EXECUTABLE_PATH2" | sed -n '2,$p' | awk '/\// {print $1}' | grep -Ev '^/usr/lib|^/System/Library')

if [ -z "$DEPENDENCIES" ]; then
    echo "myvideo没有非系统库依赖。"
    #exit 0
else
    echo "myvideo非系统库依赖列表:"
    echo "$DEPENDENCIES"

    for DEP in $DEPENDENCIES; do
        LIB_BASENAME=$(basename "$DEP")
        TARGET_PATH="$FRAMEWORKS_PATH2/$LIB_BASENAME"
        if [ ! -f "$TARGET_PATH" ]; then
            echo "cp $TARGET_PATH"
            cp "$DEP" "$FRAMEWORKS_PATH2/"
            echo "更新动态库路径:$DEP -> @executable_path"
            install_name_tool -change "$DEP" @executable_path/$LIB_BASENAME "$EXECUTABLE_PATH2"
        fi
    done
    echo "myvideo所有动态库处理完成"
fi
