use std::env;
use std::fs;
use std::fs::File;
use std::io::BufReader;
use std::io::Read;
use std::io::Write;
use std::os::unix::fs::PermissionsExt;
use std::path::PathBuf;
use std::vec;

/* Implementation of pwd function */
fn pwd() {
    let verify_path = env::current_dir();
    /* verifying if the path returned by env::current_dir is valid */
    match verify_path {
        Ok(path) => println!("{}", path.display()),
        Err(_) => std::process::exit(-1),
    }
}

/* Implementation of echo function */
fn echo(args: &[String]) {
    /* checking if there are arguments for the function */
    if args.len() <= 2 {
        std::process::exit(-10);
    }
    /* checking for -n flag */
    if args[2] == "-n" {
        print!("{}", args[3]);
        for arg in &args[4..] {
            print!(" {}", arg);
        }
    } else {
        print!("{}", args[2]);
        for arg in &args[3..] {
            print!(" {}", arg);
        }
        print!("\n");
    }
}

/* Implementation of cat function */
fn cat(args: &[String]) {
    /* checking if there are arguments for the function */
    if args.len() <= 2 {
        std::process::exit(-20);
    }
    for arg in &args[2..] {
        let ver_file = File::open(arg);
        /* verifying if file can be open, or else returning an error */
        match ver_file {
            Ok(file) => {
                /* reading the file content in a string and printing it */
                let mut buffer = BufReader::new(file);
                let mut text = String::new();
                let _ = buffer.read_to_string(&mut text);
                print!("{}", text);
            }
            Err(_) => {
                std::process::exit(-20);
            }
        }
    }
}

/* Implementation of mkdir function */
fn mkdir(args: &[String]) {
    /* checking if there are arguments for the function */
    if args.len() <= 2 {
        std::process::exit(-30);
    }
    /* making directories for each argument of the mkdir function */
    for arg in &args[2..] {
        let dir = fs::create_dir(&arg);
        /* checking if the creation of the directory was successful */
        match dir {
            Ok(_) => (),
            Err(_) => std::process::exit(-30),
        }
    }
}

/* Implementation of mv function */
fn mv(args: &[String]) {
    /* checking if there are enough arguments for the function */
    if args.len() <= 3 {
        std::process::exit(-40);
    } else {
        let mv_file = std::fs::rename(&args[2], &args[3]);
        /* checking if the file was renamed/ moved successfully */
        match mv_file {
            Ok(_) => (),
            Err(_) => std::process::exit(-40),
        }
    }
}

/* Implementation of ln function */
fn ln(args: &[String]) {
    /* checking if there are enough arguments for the function */
    if args.len() <= 3 {
        std::process::exit(-50);
    }
    if args.len() == 4 {
        let hard_link = std::fs::hard_link(&args[2], &args[3]);
        match hard_link {
            Ok(_) => (),
            Err(_) => std::process::exit(-50),
        }
    } else if args.len() == 5 {
        if args[2] == "-s" || args[2] == "--symbolic" {
            let sym_link = std::os::unix::fs::symlink(&args[3], &args[4]);
            match sym_link {
                Ok(_) => (),
                Err(_) => std::process::exit(-50),
            }
        } else {
            println!("Invalid command");
            std::process::exit(-1);
        }
    } else {
        println!("Invalid command");
        std::process::exit(-1);
    }
}

/* Implementation of rmdir function */
fn rmdir(args: &[String]) {
    /* checking if there are enough arguments */
    if args.len() <= 2 {
        std::process::exit(-60);
    }
    for arg in &args[2..] {
        /* removing directory and checking if it was successful */
        let check_rmdir = std::fs::remove_dir(arg);
        match check_rmdir {
            Ok(_) => (),
            Err(_) => std::process::exit(-60),
        }
    }
}

/* Implementation of rm function */
fn rm(args: &[String]) {
    /* checking the number of arguments */
    if args.len() <= 2 {
        std::process::exit(-70);
    }

    /* checking if command is valid */
    if args.len() == 3 && args[2] == "-r" {
        println!("Invalid command");
        std::process::exit(-1);
    }

    let mut flag = true;

    /* checking if empty directories will be deleted as well */
    if args[2] == "-d" || args[2] == "--dir" {
        for arg in &args[3..] {
            let path = PathBuf::from(arg);
            let is_dir = path.is_dir();
            let is_file = path.is_file();

            /* checking wheter the path is a file or a directory */
            if is_file == true {
                let rm_file = std::fs::remove_file(arg);
                match rm_file {
                    Ok(_) => (),
                    Err(_) => std::process::exit(-70),
                }
            } else if is_dir == true {
                /* only empty directories will be deleted */
                let rm_dir = std::fs::remove_dir(arg);
                match rm_dir {
                    Ok(_) => (),
                    Err(_) => (),
                }
            }
        }
    /* checking if the deleting will be recursive */
    } else if args[2] == "-r" || args[2] == "-R" || args[2] == "--recursive" {
        for arg in &args[3..] {
            if arg == "-d" {
                continue;
            }
            let path = PathBuf::from(arg);
            let is_dir = path.is_dir();
            let is_file = path.is_file();

            /* checking if path is file or directory and deleting it */
            if is_file == true {
                let rm_file = std::fs::remove_file(arg);
                match rm_file {
                    Ok(_) => (),
                    Err(_) => std::process::exit(-70),
                }
            } else if is_dir == true {
                let rm_dir = std::fs::remove_dir_all(arg);
                match rm_dir {
                    Ok(_) => (),
                    Err(_) => std::process::exit(-70),
                }
            } else {
                flag = false;
            }
            if flag == false {
                std::process::exit(-70);
            }
        }
    } else {
        /* deleting only files */
        for arg in &args[2..] {
            let path = PathBuf::from(arg);
            let is_file = path.is_file();

            if is_file == true {
                let rm_file = std::fs::remove_file(path);
                match rm_file {
                    Ok(_) => (),
                    Err(_) => std::process::exit(-70),
                }
            } else {
                /* flag becomes false if at least one entry is not a file */
                flag = false;
            }
        }
        /* returning error if there are directories amongst the entries */
        if flag == false {
            std::process::exit(-70);
        }
    }
}

/* Implementation of ls function */

/* helping function that prints content without recursivity */
fn ls_path_directory(path: &String, all_flag: bool) {
    let dir_iterator = fs::read_dir(path);
    match dir_iterator {
        Ok(iter) => {
            if all_flag == true {
                println!(".");
                println!("..");
            }
            for entry in iter {
                match entry {
                    Ok(valid) => {
                        let name_entry = valid.file_name().into_string().unwrap();
                        if all_flag == false {
                            if !name_entry.starts_with(".") {
                                println!("{}", name_entry);
                            }
                        } else {
                            println!("{}", name_entry);
                        }
                    }
                    Err(_) => std::process::exit(-80),
                }
            }
        }
        Err(_) => std::process::exit(-80),
    }
}

/* helping function that prints content with recursivity */
fn ls_path_recursive(path: &PathBuf, all_flag: bool) {
    let dir_iterator = fs::read_dir(path);
    println!("{}:", path.display());

    /* vectors for entries */
    let mut directories: Vec<String> = vec![];
    let mut files: Vec<String> = vec![];
    let mut path_dir: Vec<PathBuf> = vec![];

    match dir_iterator {
        Ok(iter) => {
            if all_flag == true {
                println!(".");
                println!("..");
            }
            for entry in iter {
                match entry {
                    Ok(valid) => {
                        let name_entry = valid.file_name().into_string().unwrap();
                        if valid.path().is_dir() {
                            directories.push(name_entry);
                            path_dir.push(valid.path());
                        } else if valid.path().is_file() {
                            files.push(name_entry);
                        }
                    }
                    Err(_) => std::process::exit(-80),
                }
            }
        }
        Err(_) => std::process::exit(-80),
    }
    directories.sort();
    files.sort();
    path_dir.sort();
    for entry in files {
        println!("{}", &entry);
    }
    for entries in &directories {
        println!("{}", entries);
    }
    for entry in path_dir {
        println!("");
        ls_path_recursive(&entry, all_flag);
    }
}

/* main ls body */
fn ls(args: &[String]) {
    let current_dir = String::from(".");
    if args.len() == 2 {
        ls_path_directory(&current_dir, false);
    }
    if args.len() == 3 {
        if args[2] == "-a" || args[2] == "-all" {
            ls_path_directory(&current_dir, true);
        } else if args[2] == "-r" || args[2] == "-R" {
            ls_path_recursive(&PathBuf::from(&current_dir), false);
        } else {
            let check_if_file = PathBuf::from(&args[2]);
            if check_if_file.is_dir() {
                ls_path_directory(&args[2], false);
            } else if check_if_file.is_file() {
                println!("{}", check_if_file.display());
            } else {
                std::process::exit(-80);
            }
        }
    }
    if args.len() == 4 {
        if args[2] == "-a" || args[2] == "-all" {
            ls_path_directory(&args[3], true);
        } else if args[2] == "-r" || args[2] == "-R" {
            ls_path_recursive(&PathBuf::from(&args[3]), false);
        }
    }
    if args.len() == 5 {
        ls_path_recursive(&PathBuf::from(&args[4]), true);
    }
}

/* Implementation of cp */
/* function for copying a file */
fn copy_file(source: &PathBuf, destination: &PathBuf) {
    if destination.exists() {
        if destination.is_dir() {
            let path = destination.join(source.clone().file_name().unwrap());
            let _ = std::fs::File::create(&path);
            let _ = std::fs::copy(&source, &path);
        } else if destination.is_file() {
            let _ = std::fs::copy(&source, &destination);
        }
    } else {
        let _ = std::fs::File::create(&destination);
        let _ = std::fs::copy(&source, &destination);
    }
}

/* function for copying and renaming a directory */
fn copy_directory_rec(source: &PathBuf, destination: &PathBuf) {
    if destination.exists() {
        if destination.is_file() {
            std::process::exit(-90);
        }
    } else {
        let _ = fs::create_dir(&destination);
    }

    let dir_iterator = fs::read_dir(&source);
    let mut path_dir: Vec<PathBuf> = vec![];
    let mut path_file: Vec<PathBuf> = vec![];
    match dir_iterator {
        Ok(iter) => {
            for entry in iter {
                match entry {
                    Ok(valid) => {
                        if valid.path().is_dir() {
                            path_dir.push(valid.path());
                        } else if valid.path().is_file() {
                            path_file.push(valid.path());
                        }
                    }
                    Err(_) => std::process::exit(-90),
                }
            }
        }
        Err(_) => std::process::exit(-90),
    }
    for entry in path_file {
        copy_file(&entry, &destination);
    }
    for entry in path_dir {
        let new_dir = destination
            .clone()
            .join(&entry.clone().file_name().unwrap());
        let _ = fs::create_dir(&new_dir).unwrap();
        copy_directory_rec(&entry, &new_dir);
    }
}

/* main cp implementation */
fn cp(args: &[String]) {
    if args.len() <= 3 {
        std::process::exit(-90);
    } else if args.len() == 4 {
        let path = PathBuf::from(&args[2]);
        if path.is_file() {
            let destination = PathBuf::from(&args[3]);
            copy_file(&path, &destination);
        } else {
            std::process::exit(-90);
        }
    } else if args.len() == 5 {
        if args[2] == "-r" || args[2] == "-R" || args[2] == "--recursive" {
            let path = PathBuf::from(&args[3]);
            if path.is_file() {
                let destination = PathBuf::from(&args[4]);
                copy_file(&path, &destination);
            } else if path.is_dir() {
                let destination = PathBuf::from(&args[4]);
                copy_directory_rec(&path, &destination)
            }
        }
    }
}

/* Implementation of touch */
fn touch(args: &[String]) {
    if args.len() <= 2 {
        std::process::exit(-100);
    }
    if args.len() == 3 {
        let path = PathBuf::from(&args[2]);
        if path.exists() {
            let random_path = PathBuf::from("random.txt");
            copy_file(&path, &random_path);
            let _ = std::fs::remove_file(&path);
            copy_file(&random_path, &path);
            let _ = std::fs::remove_file(random_path);
        } else {
            let _ = fs::OpenOptions::new().create(true).write(true).open(path);
        }
    }
    if args.len() == 4 {
        if args[2] == "-c" || args[2] == "--no-creat" {
            let path = PathBuf::from(&args[3]);
            let _ = fs::OpenOptions::new().create(false).write(true).open(path);
        }
        if args[2] == "-a" {
            let path = PathBuf::from(&args[3]);
            let _ = fs::read_to_string(path);
        }
        if args[2] == "-m" {
            let path = PathBuf::from(&args[3]);
            let mut file = fs::OpenOptions::new()
                .create(true)
                .append(true)
                .open(path)
                .unwrap();
            let length = file.metadata().unwrap().len();
            let _ = write!(file, "");
            let _ = file.set_len(length);
        }
    }
}

/* Implementation of chmod */
fn chmod(args: &[String]) {
    if args.len() <= 3 || args.len() >= 5 {
        std::process::exit(-25);
    }
    let path = PathBuf::from(&args[3]);
    let mut perms = fs::metadata(&path).unwrap().permissions();

    let copy = args[2].clone();
    let mut check_if_numbers = false;
    let mut check_if_add = false;
    let mut check_if_sub = false;
    let mut permission_mask: u32 = 0;
    let mut permission_value: u32 = 0;

    let verify_valid_cmd = args[2].clone().chars().next().unwrap();
    if verify_valid_cmd.is_alphanumeric() == false {
        println!("Invalid command");
        std::process::exit(-1);
    }

    for character in copy.chars() {
        if character.is_ascii_digit() {
            check_if_numbers = true;
            break;
        }
        match character {
            'u' => permission_mask += 0o100,
            'g' => permission_mask += 0o010,
            'a' => permission_mask += 0o111,
            'o' => permission_mask += 0o001,
            'r' => permission_value += 0o4,
            'w' => permission_value += 0o2,
            'x' => permission_value += 0o1,
            '+' => check_if_add = true,
            '-' => check_if_sub = true,
            _ => {
                println!("Invalid command");
                std::process::exit(-1);
            }
        }
    }
    if check_if_numbers == true {
        perms.set_mode(u32::from_str_radix(&args[2], 8).unwrap());
        let check_perm_change = fs::set_permissions(path, perms);
        match check_perm_change {
            Ok(_) => (),
            Err(_) => std::process::exit(-25),
        }
    } else {
        if check_if_add == true {
            perms.set_mode(perms.mode() | (permission_value * permission_mask));
            let check_perm_change = fs::set_permissions(path, perms);
            match check_perm_change {
                Ok(_) => (),
                Err(_) => std::process::exit(-25),
            }
        } else if check_if_sub == true {
            perms.set_mode(perms.mode() & (0o777 - (permission_value * permission_mask)));
            let check_perm_change = fs::set_permissions(path, perms);
            match check_perm_change {
                Ok(_) => (),
                Err(_) => std::process::exit(-25),
            }
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() > 1 {
        match args[1].as_str() {
            "pwd" => pwd(),
            "echo" => echo(&args),
            "cat" => cat(&args),
            "mkdir" => mkdir(&args),
            "mv" => mv(&args),
            "ln" => ln(&args),
            "rmdir" => rmdir(&args),
            "rm" => rm(&args),
            "ls" => ls(&args),
            "cp" => cp(&args),
            "touch" => touch(&args),
            "chmod" => chmod(&args),
            _ => {
                println!("Invalid command");
                std::process::exit(-1);
            }
        }
    }
}
