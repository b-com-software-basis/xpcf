#!/bin/bash

version="0.0.0"
force=false
versionFound=false
cleanFound=false
templateFound=false
template=""

display_usage() { 
    echo 'This script generates or replaces files from all template files (*.template) in the directory and its subdirectory'
    echo 'replacing marker "<version>" by the version (default version "0.0.0").'
    echo
    echo 'update_version.sh [options] --version|-v <version_number>'
    echo 'options:'
    echo '   -   --help or -h            display usage'
    echo '   -   --force or -f           force generation (replace previous file)'
    echo '   -   --clean or -c           remove all generated files from template'
    echo '   -   --template or -t        generate the file only for the template given in parameter'

    exit 1
} 

# Check parameters
for arg
do
    if [ "$versionFound" = true ]
    then
        version=$arg
    fi
    if [ "$templateFound" = true ]
    then
        template=$arg
    fi
    if [[ $arg == "--help" || $arg == "-h" ]] 
    then
        display_usage
        exit 0
    fi
    if [[ $arg == "--force" || $arg == "-f" ]] 
    then
        force=true
    fi
    if [[ $arg == "--version" || $arg == "-v" ]] 
    then
        versionFound=true
    fi
    if [[ $arg == "--template" || $arg == "-t" ]] 
    then
        templateFound=true
    fi
    if [[ $arg == "--clean" || $arg == "-c" ]] 
    then
        clean=true
    fi
done

if [[ "$templateFound" == false ]]
then
    for i in $(find . -type f -name "*.template")
    do
        # Remove template extension
        outFile=${i%.template}

        if [[ "$clean" == true ]]
        then
            rm $outFile
            echo "removed file $outFile"
        elif [[ ! -f $outFile || "$force" == true ]]
        then
            sed 's/<version>/'$version'/g' $i > $outFile
            echo "generated file $outFile"
        else 
            echo "no replacement of $outFile"
        fi

        if [[ $outFile == *.sh ]]
        then
            chmod +x $outFile
        fi
    done
elif [[ ! "$template" == "" ]]
then
    # Remove template extension
    outFile=${template%.template}

    sed 's/<version>/'$version'/g' $template > $outFile
    echo "generated file $outFile"
else
    echo "error in template parameters: $template"
fi

exit 0

