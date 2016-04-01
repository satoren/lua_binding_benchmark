#!/bin/bash

git config user.name "Travis CI"
git config user.email "satoreyo@hotmail.com"

git add -u
git status
git branch
git commit -m "Deploy to GitHub Pages"
git push --force --quiet "https://${GH_TOKEN}@${GH_REF}" master:gh-pages > /dev/null 2>&1

