Param (
    [string]$SourcePath, 
    [string]$DestinationPath,
    [string]$FileExtension
)

Get-ChildItem -Path $SourcePath -Filter "*$FileExtension" | Move-Item -Destination $DestinationPath
