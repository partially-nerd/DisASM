#/bin/zsh

# dependencies
depends=(
  g++
  )

for depend in $depends
do
  if command -v $depend > /dev/null 2>&1; then
    echo "$(whereis $depend) found"
  else
    echo "\033[31m$depend missing.\033[0m"
  fi
done

# remake build dir
rm -rf ./build
mkdir -p ./build

# build
files=(
  "src/main.c++"
  )

for file in $files
do
  path=$(dirname -- "$file")
  path=${path//\//.}
  name=$(basename -s .c++ $file)
  echo "Building ./build/$path.$name..."
  g++ $file -o "./build/$path.$name"
  echo "Built ./build/$path.$name."
done

