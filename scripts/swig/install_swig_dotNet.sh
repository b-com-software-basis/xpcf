#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu" ]]; then
echo "Installing microsoft package signing key"
wget https://packages.microsoft.com/config/debian/11/packages-microsoft-prod.deb -O packages-microsoft-prod.deb
sudo dpkg -i packages-microsoft-prod.deb
rm packages-microsoft-prod.deb

echo "Installing .net sdk"
sudo apt-get update;
sudo apt-get install -y apt-transport-https && sudo apt-get update && sudo apt-get install -y dotnet-sdk-6.0
elif [[ "$OSTYPE" == "darwin"* ]]; then
echo "Download and install dotnet core package from https://dotnet.microsoft.com/en-us/download"
fi

echo "Installing swig"
brew install swig
