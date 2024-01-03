param(
    [int]$Cores = 0
)

if ($Cores -eq 0) {
    $Cores = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
}

Write-Output "Using $Cores cores..."

$Dir = "."

# Traverse recursivelly into ../vrg directory and run clang-format in parallel with at most $Cores instances.
Get-ChildItem -Path $Dir\* -Include *.cpp, *.h -Recurse  | ForEach-Object -Process {
    $file = $_
    $running = @(Get-Job | Where-Object { $_.State -eq "Running" })
    if ($running.Count -le $Cores) {
        Write-Output "Formatting: $file"
        Start-Job {
            clang-format -i $args[0]
        } -ArgumentList $file | Out-Null
    } else {
        $running | Wait-Job -Any | Out-Null
    }
    Get-Job | Receive-Job
}

Wait-Job * | Out-Null
Remove-Job -State Completed
