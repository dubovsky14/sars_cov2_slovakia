Still under heavy development.

Compile and run:
```

# create directories

mkdir data/sensitive results bil

# convert pickle database to txt file

python python/ConvertPickeToTextFile.py <path to the pickle file> data/sensitive/migrations.txt

#compile (if this does not work, there is also CMakeLists file for cmake build)

make

# run

python/plotting/main -c config/config_example.yml
```

## Plotting

Copy example configuration file:
```bash
cp config/plot_example.json config/plot.json
```

Run plotting program
```bash
python/plotting/plot -i results/results_busines_as_usual.json
```

Plot types:
  * `slovakia`
  * `by_municipality`
