opt -disable-output -passes="dot-cfg" "$1.ll"
cat .main.dot | dot -Tpdf > "$1_base.pdf"

opt -disable-output -passes="dot-cfg" "$1_opt.ll"
cat .main.dot | dot -Tpdf > "$1_opt.pdf"

