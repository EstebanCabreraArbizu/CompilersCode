#!/usr/bin/bash

#source $HOME/Documents/git/vcompilers/bin/activate
export MYDEPENDENCIES=$HOME/Apps
#export LLVMDEPENDENCIES=$HOME/Documents/git/llvm-project/build
export CLASSPATH=.:$MYDEPENDENCIES/antlr-4.13.1-complete.jar:$CLASSPATH
alias antlr4='java org.antlr.v4.Tool'
alias grun='java org.antlr.v4.gui.TestRig'

export ANTLR4HOME=$MYDEPENDENCIES/antlr4-cpp-runtime-4.13.1-source/run
export ANTLR4I=$ANTLR4HOME/usr/local/include/antlr4-runtime
export ANTLR4L=$ANTLR4HOME/usr/local/lib/

#export LLVM17I=$LLVMDEPENDENCIES/include
#export LLVM17L=$LLVMDEPENDENCIES/lib


# para compilar archivos cpp de LLVM
# 
# arch:
#   clang++ -lLLVM toy.cpp
# 
# ubuntu/debian:
#    clang++ toy.cpp $(llvm-config-15 --cxxflags) -lLLVM-15
# 