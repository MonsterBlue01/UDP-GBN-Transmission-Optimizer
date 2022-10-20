git add --all
now=$(date +"%m/%d/%Y")
git commit -m "Update $now"
git push

echo "Script finished"
