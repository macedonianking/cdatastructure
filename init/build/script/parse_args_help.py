
import argparse
import subprocess

def parse_args():
	parser = argparse.ArgumentParser();
	parser.add_argument("--aapt", type=str, required=True,
		help="Specify the aapt tool path.");
	parser.add_argument("-M", dest="manifest", required=True,
	 	help="Specify the AndroidManifest.xml path");
	parser.add_argument("-S", dest="resource_dirs", nargs="+",
		required=True,
		help="Specify the resource directories");
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

def main():
	args = parse_args();
	print(args);
	pass;

if __name__ == "__main__":
	main();
	pass;
