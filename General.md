# TODO

rapport si on a le temps

préparer soutenance d'abord pour le 28, 10 min un seul parle, que de la démo, expliquer fuse, montré que c'est mount, géré par le process fe4 et enfin montrer le code en expliquant 'echo a > a' en disant contents est liste de fe4_dirent, etc ; expliquer 'cat d/a', slides, s'entrainer, 10 minutes

renommer data_structures
utiliser get_inode_at au lieu d'accéder à inodes directement
ne pas accéder à contents directement
gérer nlink on delete, implémenter link
implémenter chmod et chown
faire des tests automatisés
comportement correct lors de mv vers un fichier existant
renommer le projet à fe4

# Documentation

https://libfuse.github.io/doxygen/structfuse__operations.html

Dans `chmod 0777 x`, le 0 n'est pas là pour indiquer que c'est de l'octal, en effet `chmod 777 x` fait la même chose. Ce premier chiffre permet de fixer suid, sgid et sticky valant respectivement 4, 2 et 1.

Le champ st_mode de struct stat est sur 24 bits : les 12 bits de poids fort indiquent le type e.g. fichier régulier ou dossier (chaque type occupe un bit différent donc il n'y a que 12 possibilités). Les 12 bits de poids faible indiquent les permissions et suid/sgid/sticky a priori comme ceci : POIDS FORT suid_sgid_sticky user_rwx group_rwx other_rwx POIDS FAIBLE
