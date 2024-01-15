# Makefile

# Répertoires
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCLUDE_DIR = includes
LD_FLAGS = -lpthread
# Listes des programmes à générer sans prefixe
PROGLIST= server client
PROGRAMMES=$(addprefix $(BIN_DIR)/, $(PROGLIST))


all: $(PROGRAMMES)
	@echo "Compilation terminee"


# Regle de compilation .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) 

# Regle de compilation .o -> .exe
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(OBJ_DIR)/avalam.o $(OBJ_DIR)/session.o $(OBJ_DIR)/data.o
	@mkdir -p $(BIN_DIR)
	@echo "Edition de liens de $<"
	@gcc $^ -o $@ $(LD_FLAGS) -g
	@echo "Creation d'un lien symbolique vers $@"
	@ln -sf $@ .

# Regle pour les librairies
$(OBJ_DIR)/avalam.o: $(SRC_DIR)/avalam.c $(INCLUDE_DIR)/avalam.h $(INCLUDE_DIR)/topologie.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) 

$(OBJ_DIR)/session.o: $(SRC_DIR)/session.c $(INCLUDE_DIR)/session.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) -DSESSION_DEBUG

$(OBJ_DIR)/data.o: $(SRC_DIR)/data.c $(INCLUDE_DIR)/data.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) -DDATA_DEBUG

$(OBJ_DIR)/lib: $(SRC_DIR)/session.o $(INCLUDE_DIR)/avalam.o $(INCLUDE_DIR)/data.o
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@ar -crs libInet.a session.o avalam.o data.o


doc:
	@echo "Creation de la documentation"
	@mkdir -p docs
	@doxygen DoxyFile
	
# Clean
clean:
	@echo "Suppression des fichiers objets et des executables"
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(OBJ_DIR)/*
	@rm -rf server client
	@rm -rf docs/*

