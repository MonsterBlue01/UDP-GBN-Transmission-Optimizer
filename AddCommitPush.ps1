git add --all
git commit -m "$(Get-Date -Format 'yyyy/MM/dd HH:mm')"
git push

Write-Host "Script finished"