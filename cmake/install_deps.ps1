Param (
    [string]$SourcePath, 
    [string]$DestinationPath,
    [string]$FileExtension
)

Get-ChildItem -Path $SourcePath -Filter "*$FileExtension" | Copy-Item -Destination $DestinationPath
