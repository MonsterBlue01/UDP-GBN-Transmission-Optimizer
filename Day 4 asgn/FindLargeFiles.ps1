$directoryPath = "C:\Users\wdsrs\OneDrive\Desktop\github repo\CyberSecurity"
$sizeThreshold = 1

$largeFiles = Get-ChildItem -Path $directoryPath -Recurse -File | Where-Object { $_.Length -gt $sizeThreshold }

$selectedProperties = $largeFiles | Select-Object FullName, @{Name="Size(MB)";Expression={$_.Length / 1MB -as [int]}}, LastWriteTime

$selectedProperties | Export-Csv -Path "LargeFilesReport.csv" -NoTypeInformation

Write-Host "Found and exported file to 'LargeFilesReport.csv'"
