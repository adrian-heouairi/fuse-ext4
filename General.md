mettre les dirent . et .. qui existent vraiment dans ext, mettre nlink
trous dans les dirent
TODO : + Implemnet chmod
       + Implment mkdir

int fe4_mknod(const char *path, mode_t mode, dev_t dev)
int (*mkdir) (const char *, mode_t);
int (*unlink) (const char *);
int (*rmdir) (const char *);
int (*rename) (const char *, const char *, unsigned int);
int (*chmod) (const char *, mode_t);
int (*chown) (const char *, uid_t, gid_t);
int (*write) (const char *, const char *, size_t, off_t, struct fuse_file_info *);
int (*truncate) (const char *, off_t);




Dans `chmod 0777 x`, le 0 n'est pas là pour indiquer que c'est de l'octal, en effet `chmod 777 x` fait la même chose. Ce premier chiffre permet de fixer suid, sgid et sticky valant respectivement 4, 2 et 1.

Le champ st_mode de struct stat est sur 24 bits : les 12 bits de poids fort indiquent le type e.g. fichier régulier ou dossier (chaque type occupe un bit différent donc il n'y a que 12 possibilités). Les 12 bits de poids faible indiquent les permissions et suid/sgid/sticky a priori comme ceci : POIDS FORT suid_sgid_sticky user_rwx group_rwx other_rwx POIDS FAIBLE
