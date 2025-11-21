mkdir -p .\build\check_code
cppcheck --enable=all --suppressions-list=.\suppress_rules.rule --xml .\src 2> .\build\check_code\report.xml
