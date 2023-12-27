LLVM_CONFIG=/usr/lib/llvm-17/bin/llvm-config

CXX=`$(LLVM_CONFIG) --bindir`/clang
CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -fPIC -fno-rtti
LDFLAGS=`$(LLVM_CONFIG) --ldflags`
IRFLAGS=-Xclang -disable-O0-optnone -fno-discard-value-names -S -emit-llvm
OPT=`$(LLVM_CONFIG) --bindir`/opt


.PHONY: all test run clean 
all: hw1.so test

test: test1.ll

hw1.so: hw1.cpp
	$(CXX) -shared -o $@ $< $(CXXFLAGS) $(LDFLAGS)

test1.ll: test1.c
	$(CXX) $(IRFLAGS) -o $@ $<

test2.ll: test2.c
	$(CXX) $(IRFLAGS) -o $@ $<

$(CASE).ll: $(CASE).c
	$(CXX) $(IRFLAGS) -o $@ $<

run: $(CASE).ll hw1.so
	$(OPT) -disable-output -load-pass-plugin=./hw1.so -passes=hw1 $<

clean:
	rm -f *.o *.ll *.so
