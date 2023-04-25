const path = require("path"),
	fs = require("fs"), 
	events = require('events'), 
	util = require('util')
const glutils = require("./glutils.js")

class Shaderman extends events.EventEmitter {
	// shaders contains a list of shaderprograms
	// indexed by the fragname
	shaders = {};
	// the folder to watch
	folder = "shaders";
	// keeps track of the list of filepaths of shaders being watched
	modified = {};
	// given a filepath, which shader names need to be reloaded?
	dependencies = {}

	constructor(gl, folder = "shaders") {
		super();
		this.folder = folder
		this.watch(gl)
	}

	// create(gl, "test") will load "test.vert.glsl" and "test.frag.glsl"
	// create(gl, "standard", "test") will load "standard.vert.glsl" and "test.frag.glsl"
	// in both cases the shader will be available under this.shaders["test"]
	// shaders can use #include "another.glsl" inside them for common code
	create(gl, vertname, fragname) {
		fragname = fragname || vertname
		let args = [vertname, fragname]
		let name = fragname
		let vertpath = path.join(this.folder, `${vertname}.vert.glsl`)
		let fragpath = path.join(this.folder, `${fragname}.frag.glsl`)
		let vertcode = fs.readFileSync(vertpath, "utf-8")
		let fragcode = fs.readFileSync(fragpath, "utf-8")

		this.addDependency(vertpath, args)
		this.addDependency(fragpath, args)

		// apply #include rules:
		const replacer = (match, filepath) => {
			filepath = path.join(this.folder, filepath)
			if (fs.existsSync(filepath)) {
				this.addDependency(filepath, args)
				return "\n"+fs.readFileSync(filepath, "utf-8")+"\n"
			}
			return "\n"
		}
		vertcode = vertcode.replace(/#include\s+["']([^"']+)["']/g, replacer);
		fragcode = fragcode.replace(/#include\s+["']([^"']+)["']/g, replacer);

		//console.log("fragcode", fragcode)

		let program = glutils.makeProgram(gl, vertcode, fragcode)
		this.shaders[name] = program
		return program
	}

	reload(gl, name) {
		if (this.shaders[name]) {
			this.shaders[name].dispose()
			return this.create(gl, name)
		}
	}

	addDependency(filepath, args) {
		// store modtime:
		this.modified[filepath] = fs.statSync(filepath).mtimeMs
		// update dependencies:
		let deps = this.dependencies[filepath] || {}
		deps[args] = true
		this.dependencies[filepath] = deps
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
				for (let args in this.dependencies[filepath]) {
					let [vertname, fragname] = args.split(",")
					console.log("reload shader", filename, fragname)
					this.shaders[fragname].dispose()
					this.create(gl, vertname, fragname)

					this.emit('reload', vertname, fragname);
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

//util.inherits(Shaderman, events.EventEmitter)

module.exports = Shaderman