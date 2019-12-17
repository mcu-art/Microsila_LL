
import sys
import os
import shutil


def replace_in_files(filenames: list, outdir: str, replacement_dict: dict):
  for filename in filenames:
    filename = outdir + filename
    contents = ''
    with open(filename, encoding='utf-8') as f:
      contents = f.read()
      for k,v in replacement_dict.items():
        contents = contents.replace(k,  v)
    with open(filename, 'w') as out:
      out.write(contents)

def create_test():
  args = sys.argv
  if len(args) == 1:
    print("Usage: python create_test.py 'your_project_name'; then copy directory to the 'tests' folder")
    return
  proj_name = args[1]
  print(f"Creating test 'proj_name' for the 'BLUE_PILL' board...")
  
  curdir = os.path.dirname(os.path.realpath(__file__))
  srcdir = curdir + f"/templates/test_template"
  outdir = curdir + f"/{proj_name}"
  #print(f"* Current dir: '{curdir}'")
  #print(f"* Output dir: '{outdir}'")
  if os.path.isdir(outdir):
    try:
      shutil.rmtree(outdir)
    except OSError:
      print (f"Error: can't delete directory {outdir}; try deleting it manually and then continue")
      return
    else:
      print (f"Successfully deleted old directory {outdir}")

  # copy files and directories that does not have replaceable content  
  try:
    shutil.copytree(srcdir, outdir)
  except OSError:
    print (f"Error: can't copy directory {srcdir} to {outdir}")
    return
  else:
    print (f"Successfully copied '{srcdir}' to '{outdir}'")

  
  # make replacements in files
  filelist = ["/MDK-ARM/@@PROJECT_NAME@@.uvprojx",
                          "/MDK-ARM/RTE/RTE_Components.h"]
  replacements = {'@@PROJECT_NAME@@': proj_name}

  replace_in_files(filelist, outdir, replacements)

  # rename files
  shutil.move(outdir + "/@@PROJECT_NAME@@.ioc", outdir + f"/{proj_name}.ioc")
  shutil.move(outdir + "/MDK-ARM/@@PROJECT_NAME@@.uvprojx", outdir + f"/MDK-ARM/{proj_name}.uvprojx")

  # copy microsila_ll files
  shutil.copy("../../mi_ll_settings.h.tpl", outdir + "/Inc/mi_ll_settings.h")
  print("Done.")

if __name__ == '__main__':
  create_test()
  