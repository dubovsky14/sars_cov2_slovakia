Still under heavy development.

Compile and run:
```

# create directories

mkdir data/sensitive results bin

# convert pickle database to txt file

python python/ConvertPickeToTextFile.py <path to the pickle file> data/sensitive/migrations.txt

#compile (if this does not work, there is also CMakeLists file for cmake build)

make

# run

bin/main -c config/config_example.yml
```
