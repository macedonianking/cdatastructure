
import argparse
import subprocess
import os
import os.path

def parse_args():
	parser = argparse.ArgumentParser();
	parser.add_argument("--aapt", type=str, required=True,
		help="Specify the aapt tool path.");
	parser.add_argument("-M", dest="manifest", required=True,
	 	help="Specify the AndroidManifest.xml path");
	parser.add_argument("-S", dest="resource_dirs", nargs="+",
		required=True,
		help="Specify the resource directories");
	parser.add_argument("-f", dest="force", action="store_true",
		default=False,
		help="force update.")
	parser.add_argument("--base-packages", dest="base_pachages",
		nargs="*", help="Specify the base packages");
	parser.add_argument("-F", dest="apk_path", type=str,
		required=True,
		help="Specify the apk path.");
	parser.add_argument("-J", dest="r_file_dir", type=str,
		required=True,
		help="Specify the R.java file dir.");
	parser.add_argument("--non-constant-id", dest="non_constant_id",
		action="store_true", default=False, 
		help="Generate non constant id");
	parser.add_argument("--depfile", type=str,
		help="Specify the depfile path.")
	parser.add_argument("-A", dest="asset_dir", type=str,
		help="Specify the asset directory.");
	return parser.parse_args();

def get_aapt_command(args):
	command = [ args.aapt ]
	command += [ "package" ]
	if args.force:
		command += [ "-f" ]
	command += [ "-M", args.manifest ]
	command += [ "-S", ";".join(args.resource_dirs) ]
	for item in args.base_pachages:
		command += [ "-I", item ]
	command += [ "-J", args.r_file_dir ]
	if args.non_constant_id:
		command += [ "--non-constant-id" ]
	if args.asset_dir:
		command += [ args.asset_dir ]
	command += [ "-F", args.apk_path ]
	return command

def walk_dir(dir_path, file_obj):
	for root, dirs, files in os.walk(dir_path, topdown=True):
		for filename in files:
			abs_file = os.path.abspath(os.path.join(root, filename))
			print(abs_file, file=file_obj, end=' ')

def gen_depfile(args):
	with open(args.depfile, "w") as file_obj:
		print("%s : " % (args.apk_path), end="",file=file_obj);
		for item in args.resource_dirs:
			walk_dir(item, file_obj);
	pass;

def main():
	args = parse_args();
	command = get_aapt_command(args);
	subprocess.check_output(command, shell=True);
	if args.depfile:
		gen_depfile(args);
	pass

if __name__ == "__main__":
	main();
	pass;
