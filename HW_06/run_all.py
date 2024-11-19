import subprocess

INIT_2D = "initial_2D.dat"
OUT_FINAL = "stencil_final.dat"
ALL_OUT = "stencil_all.dat"
MOVIE = "100x100x500.mp4"
ROWS = 100
COLS = 100
ITTER = 500

def make_clean_all():
    subprocess.run(["make", "clean", "all"], check = True)

def run_make2D():
    cmd = ["./make-2d", str(ROWS), str(COLS), INIT_2D]
    subprocess.run(cmd, check = True)

# def run_print2D():
#     cmd = ["./print-2d", INIT_2D]
#     subprocess.run(cmd, check = True)

def run_stencil2D():
    cmd = ["./stencil-2d", str(ITTER), INIT_2D, OUT_FINAL, ALL_OUT]
    subprocess.run(cmd, check = True)

def run_make_movie():
    cmd = ["python3", "make-movie.py", ALL_OUT, MOVIE]
    subprocess.run(cmd, check = True)

def main():
    make_clean_all() 
    
    print("--------------- RUNNING MAKE-2D --------------")
    run_make2D()

    print("------------- RUNNING STENCIL-2D -------------")
    run_stencil2D()
    
    print("--------------- MAKING MOVIE -----------------")
    run_make_movie()


if __name__ == "__main__":
    main()



    