import argparse
import subprocess
import sys
import re

PROJECT_NAME = "PCC - Paulo C Compiler"

class Output(list):
	indent_level = 0
	def __init__(self, f):
		super()
		if type(f) == list:
			self.extends(f)
		else:
			self.append(f)

	def writeln(self, text, indent_level=None):
		text = str(text)
		if indent_level != None:
			self.indent_level = indent_level
		else:
			indent_level = self.indent_level

		if text == "":
			indent_level = 0

		text = bytes((get_indent(indent_level) + text + "\n").encode("utf-8"))
		for f in self:
			f.write(text)

def get_indent(indent_level):
	return "\t" * indent_level

def print_variable(_type=None, name=None):
	string = ""

	if _type != None:
		string = _type
		if _type[-1] != "*":
			string += " "

	if name != None:
		string += name

	return string

class Comment:
	@staticmethod
	def create_comment(text, indent_level=0):
		return get_indent(indent_level) + "// " + text

class Comment_Classic(Comment):
	@staticmethod
	def inline(text, indent_level):
		return get_indent(indent_level) + "/* " + text[0] + " */"

	@classmethod
	def multiline(cls, text, indent_level):
		return cls.create_comment(text, indent_level, False)

	@staticmethod
	def create_comment(text, indent_level=0, guess=True):
		if type(text) != list:
			text = text.split("\\n")
			if len(text) == 1 and guess:
				return Comment_Classic.inline(text[0], indent_level)

		comment = get_indent(indent_level) + "/*\n"
		for line in text:
			comment += get_indent(indent_level) + " * " + line + "\n"
		comment += get_indent(indent_level) + " */"

		return comment

class Comment_Doc(Comment_Classic):
	@staticmethod
	def inline(text, indent_level):
		return get_indent(indent_level) + "/// " + text[0]

	@staticmethod
	def create_comment(text, indent_level, guess=True):
		if type(text) != list:
			text = text.split("\\n")
			if len(text) == 1 and guess:
				return Comment_Doc.inline(text, indent_level)

		comment = get_indent(indent_level) + "/**\n"
		for line in text:
			comment += get_indent(indent_level) + " * " + line + "\n"
		comment += get_indent(indent_level) + " */"

		return comment


class Name:
	def __init__(self, name):
		self.capital = name.capitalize()
		self.lower = name.lower()
		self.upper = name.upper()

		self.header = self.lower + ".h"
		self.source = self.lower + ".c"

		self.guard = self.replace_non_alpha(self.upper) + "_H"

		self.struct = self.replace_non_alpha(self.lower) + "_t"
		self.instance = self.replace_non_alpha(self.lower)

	@staticmethod
	def replace_non_alpha(name):
		return re.sub("\W", "_", name)

class Attribute:
	indent_level = 1
	def __init__(self, attribute, indent_level=1):
		values = attribute.strip().split(":", 2)
		self.name = values[0].strip()
		self.type = values[1].strip()

		if len(values) > 2:
			self.description = values[2].strip()
		else:
			self.description = ""

		self.indent_level = indent_level

	def get_indent(self):
		return "\t" * self.indent_level

	def __str__(self):
		return print_variable(self.type, self.name)

	def declaration(self):
		# Description
		string = Comment_Doc.create_comment(self.description, self.indent_level) + "\n"

		string += self.get_indent() + str(self) + ";\n"

		return string

	def header_get(self, name):
		return "%s%s_get_%s(const %s *%s)" % (print_variable(self.type), name.lower, self.name, name.struct, name.instance)

	def header_set(self, name):
		return "void %s_set_%s(%s *%s, %s)" % (name.lower, self.name, name.struct, name.instance, str(self))

	def source_get(self, name):
		string = self.header_get(name) + " {\n"
		string += ("\treturn %s->%s;\n" % (name.instance, self.name))
		string += "}"

		return string

	def source_set(self, name):
		string = self.header_set(name) + " {\n"
		string += ("\t%s->%s = %s;\n" % (name.instance, self.name, self.name))
		string += "}"

		return string


if __name__ == "__main__":
	parser = argparse.ArgumentParser()

	parser.add_argument("name", help="Name of the module")
	parser.add_argument("-D", "--description", help="Module description")
	parser.add_argument("-d", "--depends", help="List of modules that it depends on")
	parser.add_argument("-a", "--attributes", help="Create a struct with the attributes")
	parser.add_argument("-v", "--verbose", type=int, help="Print the file to stdout. 1=header, 2=source, 3=both", default = 0)
	parser.add_argument("--header", help="Specify header name. If the file already exists, it will be used as is")

	args = parser.parse_args()

	name = Name(args.name)
	if args.header:
		name.header = args.header

	author = subprocess.getoutput("git config --get user.name").strip()
	email = subprocess.getoutput("git config --get user.email").strip()


	def header(filename):
		comment = [PROJECT_NAME, "", filename, ""]
		if args.description:
			comment.extend([args.description, ""])
		comment.extend([author, email])

		return Comment_Doc.create_comment(comment, 0)

	def constructor(attributes):
		return "%s new_%s(%s)" % (name.struct, name.lower, ", ".join(map(str, attributes)))

	# .h
	if args.header == None:
		with open("include/" + name.header, "wb") as f:
			files = Output(f)
			if args.verbose & 1:
				files.append(sys.stdout)

			files.writeln(header(name.header) + "\n")

			# Include Guard
			files.writeln("#ifndef " + name.guard)
			files.writeln("#define " + name.guard + "\n")

			# Includes
			if args.depends:
				dependencies = args.depends.split(",")
				for dependency in dependencies:
					files.writeln('#include "%s.h"' % (dependency.strip()))
				files.writeln("")


			# Struct
			if args.attributes:
				files.writeln("typedef struct %s {" % (name.struct))

				attributes = [Attribute(attr) for attr in args.attributes.split(";")]
				for attr in attributes:
					files.writeln(attr.declaration())

				files.writeln("} %s;\n" % (name.struct))

				# Functions
				files.writeln(constructor(attributes) + ";\n")

				for attr in attributes:
					files.writeln(attr.header_get(name) + ";\n")
					files.writeln(attr.header_set(name) + ";\n")

			files.writeln("#endif " + Comment.create_comment(name.guard, 0))

	# .c
	with open("src/" + name.source, "wb") as f:
		files = Output(f)
		if args.verbose & 2:
			files.append(sys.stdout)

		files.writeln(header(name.source) + "\n")

		# Include
		files.writeln('#include "%s"' % (name.header))

		# Functions
		if args.attributes:
			files.writeln("")

			attributes = [Attribute(attr) for attr in args.attributes.split(";")]

			files.writeln(constructor(attributes) + " {")
			files.writeln("%s %s;\n" % (name.struct, name.instance), 1)

			for attr in attributes:
				files.writeln("%s_set_%s(&%s, %s);" % (name.lower, attr.name, name.instance, attr.name))

			files.writeln("")
			files.writeln("return %s;" % (name.instance))
			files.writeln("}", 0)

			for attr in attributes:
				files.writeln("\n" + attr.source_get(name) + "\n")
				files.writeln(attr.source_set(name))
