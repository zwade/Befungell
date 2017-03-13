const fs = require("fs")

let w = (ch, color = "white") => {
	let col = "\033[38;5;";
	let num;

	switch (color) {
		case "teal":
			num = 48
			break
		case "white":
			num = 15
			break
		case "green":
			num = 40
			break
		case "red":
			num = 1
			break
		case "crimson":
			num = 124
			break
		case "purple":
			num = 162
			break
		case "orange":
			num = 9
			break
		case "lgreen":
			num = 46
			break
		case "yellow":
			num = 220
			break
		case "black":
			num = 16
			break
		case "pink":
			num = 213
			break
		default:
			num = 196
			break
	}

	col += num + "m"
	process.stdout.write(col+ch)
}


fs.readFile(process.argv[2], function(err, data) {
	data = data.toString()
	console.log("")

	for (d of data) {
		switch (d) {
			case ">":
			case "<":
			case "^":
			case "v":
			case "?":
			case "@":
				w(d, "lgreen")
				break
			case "+":
			case "-":
			case "/":
			case "*":
			case "%":
			case "!":
			case "`":
				w(d, "purple")
				break
			case "_":
			case "|":
				w(d, "red")
				break
			case "\"":
				w(d, "crimson")
				break
			case ":":
			case "\\":
			case "$":
			case "#":
			        w(d, "yellow")
				break
			case ".":
			case ",":
			case "p":
			case "g":
			case "~":
			case "&":
				w(d, "orange")
				break
			case "=":
			case "{":
			case "}":
				w(d, "white")
				break
			default:
				if (!isNaN(d)) {
					w(d, "pink") 
				} else {
					w(d, "red")
				}
		}
	}	
})

