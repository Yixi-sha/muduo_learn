.PHONY : all clean rebuild  compile  link test 

TYPE := cpp
CC := g++
DEBUG:=true
ifeq ($(DEBUG),true)
CFLAGS += -g -std=c++11 
endif

LFLAGS = -lpthread -rdynamic

Moudles := muduo
Target := app.out


Build_dir := build

Moudle_libs := $(addsuffix .a, $(Moudles))
Moudle_libs := $(addprefix $(Build_dir)/, $(Moudle_libs))

Moudle_dirs := $(addprefix $(Build_dir)/, $(Moudles))
Dirs := $(Build_dir) $(Moudle_dirs)

all : $(Target)

$(Target) : compile link 

link : $(Moudle_libs) $(Build_dir)/main.o
	$(CC)  -o $(Target) -Xlinker "-(" $^  -Xlinker "-)" $(LFLAGS)

$(Build_dir)/main.o : main.$(TYPE)
	$(CC) $(CFLAGS) -o  $@ -c  $^

compile : $(Build_dir) $(Moudle_dirs)
	set -e ; \
	for dir in $(Moudles) ; \
	do \
		cd $$dir && \
		$(MAKE) all Moudles:=$(Moudles) DEBUG:=$(DEBUG) && \
		cd .. ; \
	done


$(Dirs) :
	mkdir $@

test:
	@echo "Moudle_dirs => $(Moudle_dirs)"

clean :
	rm *.out $(Build_dir) -rf 

rebuild : clean all