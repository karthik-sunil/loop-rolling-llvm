
# ACTION REQUIRED: Ensure that the path to the library and pass name are correct.
clear
sh build.sh


PATH2LIB="build/rollingpass/rollingpass.so"
PASS="loop-roll"
BENCH=tests/${1}.c


clang -emit-llvm -S ${BENCH} -Xclang -disable-O0-optnone -o ${1}.ll
opt -S -load-pass-plugin="${PATH2LIB}" -passes="${PASS},dot-cfg" ${1}.ll -o ${1}_opt.ll
# opt -disable-output -S -passes="dot-cfg" ${1}.ll

# code ${2}.pdf

