using System.Text.RegularExpressions;

namespace c11tester_tools;

internal class ExecutionLogParser
{
    private static void Main(string[] args)
    {
        var input = ReadMultiLineInput();
        var executions = ParseLog(input);
        Console.WriteLine($"Parsed {executions.Count}");
    }

    public class Execution
    {
        public int ExecutionNumber { get; set; }
        public List<string> ProgramOutput { get; set; } = [];
        public List<TraceEntry> ExecutionTrace { get; set; } = [];
        public uint Hash { get; set; }
    }

    public class TraceEntry
    {
        public int Id { get; set; }
        public int ThreadId { get; set; }
        public string ActionType { get; set; }
        public string MemoryOrder { get; set; }
        public string Location { get; set; }
        public string Value { get; set; }
        public string Rf { get; set; }
        public string Cv { get; set; }
    }

    public static List<Execution> ParseLog(string input)
    {
        var executions = new List<Execution>();
        var lines = input.Split(['\n'], StringSplitOptions.RemoveEmptyEntries);

        Execution currentExecution = null;
        var readingProgramOutput = false;
        var readingTrace = false;

        var traceRegex = new Regex(@"^\s*(\d+)\s+(\d+)\s+([\w\s]+)\s+([\w_]+)\s+([0-9A-Fa-fx]+)\s+([0-9A-Fa-fx]+)\s+([\d\s]*)\s+\(([\d,\s]*)\)");

        foreach (var line in lines)
        {
            if (line.StartsWith("Program output from execution"))
            {
                currentExecution = new Execution { ExecutionNumber = executions.Count + 1 };
                executions.Add(currentExecution);
                readingProgramOutput = true;
                readingTrace = false;
            }
            else if (line.StartsWith("Execution trace"))
            {
                readingProgramOutput = false;
                readingTrace = true;
            }
            else if (line.StartsWith("---- END PROGRAM OUTPUT"))
            {
                readingProgramOutput = false;
            }
            else if (line.StartsWith("HASH"))
            {
                if (currentExecution != null)
                {
                    currentExecution.Hash = uint.Parse(line.Split()[1]);
                }
            }
            else if (readingProgramOutput)
            {
                currentExecution.ProgramOutput.Add(line);
            }
            else if (readingTrace && currentExecution != null)
            {
                var match = traceRegex.Match(line);
                if (match.Success)
                {
                    currentExecution.ExecutionTrace.Add(new TraceEntry
                    {
                        Id = int.Parse(match.Groups[1].Value),
                        ThreadId = int.Parse(match.Groups[2].Value),
                        ActionType = match.Groups[3].Value.Trim(),
                        MemoryOrder = match.Groups[4].Value,
                        Location = match.Groups[5].Value,
                        Value = match.Groups[6].Value,
                        Rf = match.Groups[7].Value.Trim(),
                        Cv = match.Groups[8].Value.Trim()
                    });
                }
            }
        }

        return executions;

    }

    public static string ReadMultiLineInput()
    {
        var lines = new List<string>();
        var line = Console.ReadLine();
        while (line != null && !line.StartsWith("Total executions:"))
        {
            lines.Add(line);
            line = Console.ReadLine();
        }

        lines.Add(line);
        return string.Join("\n", lines);
    }
}
