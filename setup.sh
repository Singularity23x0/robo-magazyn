#!/bin/bash

# make sure to run this script from the project root directory
root_dir=$(pwd)

# relative to root_dir
downloads_dir=downloads
dependencies_dir=dependencies
build_dir=build

# POSIX compatible
function abort_if_program_doesnt_exist() {
  if ! command -v "$1" &> /dev/null
  then
    echo "$1 does not exist on your system PATH. Make sure this executable is installed."
    exit 1
  fi
}

function install_glog() {
  version="0.6.0"

  # check if not already installed
  if [[ -d ${dependencies_dir}/glog-${version} ]]; then
    echo "Glog already installed. To rebuild remove it from dependencies dir."
    return 0
  fi


  # DOWNLOAD
  cd ${root_dir}
  mkdir -p ${downloads_dir}/glog
  cd ${downloads_dir}/glog

  # Do not download if already downloaded
  if [[ ! -e "v${version}.zip" ]]; then
    abort_if_program_doesnt_exist "wget"
    wget -v "https://github.com/google/glog/archive/refs/tags/v${version}.zip"
  fi

  abort_if_program_doesnt_exist "unzip"
  unzip "v${version}.zip"

  cd ${root_dir}

  # INSTALL
  cp -R ${downloads_dir}/glog/glog-${version} ${dependencies_dir}
  rm -fr ${downloads_dir}/glog/glog-${version}

  cd ${dependencies_dir}/glog-${version}

  # BUILD
  cmake -S . -B build -G "Unix Makefiles"
  cmake --build build

  cd ${root_dir}
}

function install_json_parser() {
  version="3.10.5"

  # check if not already installed
  if [[ -d ${dependencies_dir}/json-${version} ]]; then
    echo "Nlohmann-json parser already installed. To rebuild remove it from dependencies dir."
    return 0
  fi

  # DOWNLOAD
  cd ${root_dir}
  mkdir -p ${downloads_dir}/nlohmann-json
  cd ${downloads_dir}/nlohmann-json

  # Do not download if already downloaded
  if [[ ! -e "v${version}.zip" ]]; then
    abort_if_program_doesnt_exist "wget"
    wget -v "https://github.com/nlohmann/json/archive/refs/tags/v${version}.zip"
  fi

  abort_if_program_doesnt_exist "unzip"
  unzip "v${version}.zip"

  cd ${root_dir}

  # INSTALL
  cp -R ${downloads_dir}/nlohmann-json/json-${version} ${dependencies_dir}
  rm -fr ${downloads_dir}/nlohmann-json/json-${version}

  cd ${dependencies_dir}/json-${version}

  # BUILD
  cmake -S . -B build -DJSON_BuildTests=Off
  cmake --build build

  cd ${root_dir}
}

# create build directory
mkdir -p ${build_dir}

# create downloads directory
mkdir -p ${downloads_dir}

# create dependencies directory
mkdir -p ${dependencies_dir}

install_glog
install_json_parser

cd ${root_dir}

# setup cmake project
cmake -S . -B ${build_dir}
