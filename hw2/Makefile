LLVM_CONFIG=/usr/lib/llvm-17/bin/llvm-config

CXX=`$(LLVM_CONFIG) --bindir`/clang
CXXFLAGS=`$(LLVM_CONFIG) --cppflags` -fPIC -fno-rtti
LDFLAGS=`$(LLVM_CONFIG) --ldflags`
IRFLAGS=-Xclang -disable-O0-optnone -fno-discard-value-names -S -emit-llvm
OPT=`$(LLVM_CONFIG) --bindir`/opt


.PHONY: all test run clean debug $(CASE) 
all: hw2.so

debug: $(CASE).ll hw2_dbg.so
	$(OPT) -disable-output -load-pass-plugin=./hw2_dbg.so -passes=hw2 $<

hw2_dbg.so: hw2.cpp
	$(CXX) -shared -o $@ $< $(CXXFLAGS) -DDEBUG $(LDFLAGS)

hw2.so: hw2.cpp
	$(CXX) -shared -o $@ $< $(CXXFLAGS) $(LDFLAGS)

run: $(CASE).ll hw2.so
	$(OPT) -disable-output -load-pass-plugin=./hw2.so -passes=hw2 $<

$(CASE): $(CASE).ll

$(CASE).ll: $(CASE).c
	$(CXX) $(IRFLAGS) -o $@ $<

clean:
	rm -f *.o *.ll *.so
