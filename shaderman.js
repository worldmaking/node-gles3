const path = require("path"),
	fs = require("fs")
const glutils = require("./glutils.js")

class Shaderman {
	shaders = {};
	modified = {};
	folder = "shaders";

	constructor(gl, folder = "shaders") {
		this.folder = folder
		this.watch(gl)
	}

	create(gl, name) {
		let vertpath = path.join(this.folder, `${name}.vert.glsl`)
		let fragpath = path.join(this.folder, `${name}.frag.glsl`)
		this.modified[vertpath] = fs.statSync(vertpath).mtimeMs
		this.modified[fragpath] = fs.statSync(fragpath).mtimeMs
		let vertcode = fs.readFileSync(vertpath, "utf-8")
		let fragcode = fs.readFileSync(fragpath, "utf-8")

		// apply #include rules:
		const replacer = (match, filepath) => {
			filepath = path.join(this.folder, filepath)
			if (fs.existsSync(filepath)) {
				return "\n"+fs.readFileSync(filepath, "utf-8")+"\n"
			}
			return "\n"
		}
		vertcode = vertcode.replace(/#include\s+["']([^"']+)["']/g, replacer);
		fragcode = fragcode.replace(/#include\s+["']([^"']+)["']/g, replacer);

		// console.log(vertcode)
		// console.log(fragcode)

		let program = glutils.makeProgram(gl, vertcode, fragcode)
		this.shaders[name] = program
		return program
	}

	reload(name) {
		if (this.shaders[name]) {
			this.shaders[name].dispose()
			return this.create(name)
		}
	}

	watch(gl) {
		this.watcher = fs.watch(this.folder, (eventType, filename) => {
			const filepath = path.join(this.folder, filename); 
			const mtime = fs.statSync(filepath).mtimeMs
			if (!this.modified[filepath]) {
				// not a watched file
				return
			};
			if (this.modified[filepath] != mtime) {
				this.modified[filepath] = mtime
				let match = filename.match(/^([^\.]+)/)
				if (match && match[0]) {
					let [name] = match
					console.log("reload", filename, name)
					this.shaders[name].dispose()
					this.create(gl, name)
				}
			}
		})
	}

	dispose() {
		if (this.watcher) {
			this.watcher.close()
			this.watcher = null

			for (let k of Object.keys(this.shaders)) {
				this.shaders[k].dispose()
				this.shaders[k] = null
			}
		}
	}
}

module.exports = Shaderman