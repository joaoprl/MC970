for i in {1..3}
do
    curl -O -k https://susy.ic.unicamp.br:9999/mo644a/hist_paralelo/dados/arq$i.in
    curl -O -k https://susy.ic.unicamp.br:9999/mo644a/hist_paralelo/dados/arq$i.res
done

