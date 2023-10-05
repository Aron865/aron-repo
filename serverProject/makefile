## Project(shared_bike)
EXECUTABLE :=../shared_exe
LIBDIR     :=./third/lib
LIBS       := pthread   event protobuf mysqlclient iniparser

INCLUDES   :=./third/include

SRCDIR :=./src/    ##除了当前目录下其他源文件的目录
CC:=g++
CPPFLAGS:= -g -Wall  
CPPFLAGS +=-std=c++11
CPPFLAGS +=$(addprefix -I,$(INCLUDES)) 
CPPFLAGS += -MMD

RM-F :=rm -f  

SRCS :=$(wildcard *.cpp) $(wildcard $(addsuffix /*.cpp,$(SRCDIR)))
OBJS :=$(patsubst %.cpp,%.o, $(SRCS))
DEPS :=$(patsubst %.o,%.d, $(OBJS))
## $@ :目标文件名 $<:表示依赖文件的第一个文件名
## wildcard( *.cpp) 搜索所有满足此条件的文件
## patsubst %.cpp %.o $(dir) ,将dir中满足%.cpp 文件替换成 .o文件
## $(filter-out $(A),$(B))表示从B中过滤掉A中的内容，返回剩余内容
# MISSING_DEPS :=(filter-out $(wildcard $(DEPS)), $(DEPS))
# MISSING_DEPS_SOURCES :=$(wildcard $(patsubst %.d,%.cpp,$(MISSING_DEPS)))
.PHONY :  all  deps objs clean veryclearn rebuild info
all:  $(EXECUTABLE)
deps: $(DEPS)
objs: $(OBJS)


$(EXECUTABLE): $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))    
#$(addprefix -L.$(LIBDIR)) $(addprefix -l,$(LIBS)) -levent -pthread -lmysqlclient -lprotobuf

clean:
	@$(RM-F) ./src/*.o 
	@$(RM-F) ./src/*.d
veryclearn:
	@$(RM-F) $(EXECUTABLE)
   
rebuild:
	veryclearn all
# ifneq ($MISSING_DEPS,)
# $(MISSING_DEPS): @$(RM-F) $(patsubst %.d,.%.o,$@)
# endif
-include $(DEPS)
info :
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(MISSING_DEPS)
	@echo $(MISSING_DEPS_SOURCES)