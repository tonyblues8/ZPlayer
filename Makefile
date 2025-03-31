# 设置 C++ 编译选项
CXXFLAGS += -std=c++1z -Wno-deprecated-declarations -finput-charset=UTF-8 -fexec-charset=UTF-8 \
            $(shell /opt/wxWidgets-3.2.6/build-cocoa-debug/wx-config --cxxflags) \
            $(shell /opt/wxWidgets-3.2.6/build-cocoa-debug/wx-config --cxxflags richtext) \
            -I/opt/MariaDB/include \
            -I/opt/homebrew/opt/xz/include \
            -I/opt/homebrew/opt/zstd/include \
            -I/opt/homebrew/opt/jbigkit/include \
            -I/opt/homebrew/opt/pcre2/include \
            -I/opt/liblerc/include \
            -I/opt/libtiff/include \
            -I/opt/homebrew/opt/jpeg-turbo/include \
            -I/opt/webp/include \
            -I/opt/openssl/include

# 设置链接器选项
LDFLAGS += $(shell /opt/wxWidgets-3.2.6/build-cocoa-debug/wx-config --libs) \
           $(shell /opt/wxWidgets-3.2.6/build-cocoa-debug/wx-config --libs richtext) \
           -L/opt/MariaDB/lib \
           -L/opt/homebrew/opt/xz/lib \
           -L/opt/homebrew/opt/zstd/lib \
           -L/opt/homebrew/opt/pcre2/lib \
           -L/opt/homebrew/opt/jbigkit/lib \
           -L/opt/liblerc/lib \
           -L/opt/libtiff/lib \
           -L/opt/homebrew/opt/jpeg-turbo/lib \
           -L/opt/openssl/lib \
           -L/opt/webp/lib


# 设置框架（仅在 macOS 下需要）
ifeq ($(shell uname), Darwin)
    LDFLAGS += -framework Cocoa \
               -framework QuartzCore \
               -framework AudioToolbox \
               -framework IOKit \
               -framework Security \
               -framework OpenGL \
               -framework Carbon
endif

# 设置库文件
LIBS += /opt/MariaDB/lib/mariadb/libmariadb.a \
        /opt/MariaDB/lib/mariadb/libmariadbcpp.a \
        /opt/homebrew/opt/xz/lib/liblzma.a \
        /opt/homebrew/opt/zstd/lib/libzstd.a \
        /opt/liblerc/lib/libLerc.a \
        /opt/homebrew/opt/pcre2/lib/libpcre2-32.a \
        /opt/homebrew/opt/jpeg-turbo/lib/libjpeg.a \
        /opt/homebrew/opt/jbigkit/lib/libjbig.a \
        /opt/openssl/lib/libssl.a \
        /opt/openssl/lib/libcrypto.a \
        /opt/libtiff/lib/libtiff.a \
        /opt/webp/lib/libwebp.a \
        /opt/webp/lib/libsharpyuv.a

# 源文件和目标文件
SRC_CPP = ImageButton.cpp CustomGauge.cpp CustomButton.cpp AutoCloseDialog.cpp \
           pic/h/imageon.cpp pic/h/imageoff.cpp \
           pic/h/imageon1.cpp pic/h/imageoff1.cpp \
           pic/h/yybj.cpp pic/h/yybj2.cpp \
           pic/h/bubj.cpp pic/h/bubj2.cpp pic/h/play1.cpp \
          localv.cpp
SRC_MM = Command.mm
#OBJ = $(SRC:.cpp=.o)
OBJ = $(SRC_CPP:.cpp=.o) $(SRC_MM:.mm=.o)
TARGET = mylocalvideo

# 源文件和目标文件
SRC2_CPP = ImageButton.cpp CustomGauge.cpp CustomButton.cpp AutoCloseDialog.cpp \
           pic/h/imageon.cpp pic/h/imageoff.cpp \
           pic/h/imageon1.cpp pic/h/imageoff1.cpp \
           pic/h/yybj.cpp pic/h/yybj2.cpp \
           pic/h/bubj.cpp pic/h/bubj2.cpp pic/h/play.cpp \
           my.cpp
SRC2_MM = Command.mm
#OBJ2 = $(SRC2:.cpp=.o)
OBJ2 = $(SRC2_CPP:.cpp=.o) $(SRC2_MM:.mm=.o)
TARGET2 = myvideo

# 目标: 编译并链接两个目标
all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LIBS) -o $(TARGET)
	@echo "Build successful: $(TARGET)"

$(TARGET2): $(OBJ2)
	$(CXX) $(CXXFLAGS) $(OBJ2) $(LDFLAGS) $(LIBS) -o $(TARGET2)
	@echo "Build successful: $(TARGET2)"

# 编译每个源文件
#%.o: %.cpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 C++ 文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 Objective-C++ 文件
%.o: %.mm
	$(CXX) $(CXXFLAGS) -ObjC++ -c $< -o $@


# 清理目标
clean:
	rm -f $(OBJ) $(OBJ2) $(TARGET) $(TARGET2)
