#!/usr/bin/bash

#source $HOME/Documents/git/vcompilers/bin/activate
export MYDEPENDENCIES=$HOME/Apps
export LLVMDEPENDENCIES=$HOME/Documents/Projects/llvm-project/build
export LLVMMYDEPENDENCIES="/usr"

export CLASSPATH=.:$MYDEPENDENCIES/antlr-4.13.1-complete.jar:$CLASSPATH
alias antlr4='java org.antlr.v4.Tool'
alias grun='java org.antlr.v4.gui.TestRig'

export ANTLR4HOME=$MYDEPENDENCIES/antlr4-cpp-runtime-4.13.1-source/run
export ANTLR4I=$ANTLR4HOME/usr/local/include/antlr4-runtime
export ANTLR4L=$ANTLR4HOME/usr/local/lib/

export LLVM17I=$LLVMDEPENDENCIES/include
export LLVM17L=$LLVMDEPENDENCIES/lib

export LLVM15I=$LLVMMYDEPENDENCIES/include/llvm-15
export LLVM15IC=$LLVMMYDEPENDENCIES/include/llvm-c-15
export LLVM15L=$LLVMMYDEPENDENCIES/lib/llvm-15

# para compilar archivos cpp de LLVM
# 
# arch:
#   clang++ -lLLVM toy.cpp
# 
# ubuntu/debian:
#    clang++ toy.cpp $(llvm-config-15 --cxxflags) -lLLVM-15
# Para mostrar el AST con antlr4 usando el comando grun "name_grammar" "rule" -gui
#     sudo apt install libxrender1
#     sudo apt-get install libxtst6
#     sudo apt-get install libxi6