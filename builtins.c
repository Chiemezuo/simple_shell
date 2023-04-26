#include "shell.h"

/**
 * check_for_builtins - This function checks if the command is
 * a builtin function
 *
 * @vars: represents the variables
 *
 * Return: returns the pointer to the function or returns NULL
 *
 */

void (*check_for_builtins(vars_t *vars))(vars_t *vars)
{
	unsigned int k;
	builtins_t check[] = {
		{"exit", new_exit},
		{"env", _env},
		{"setenv", new_setenv},
		{"unsetenv", new_unsetenv},
		{NULL, NULL}
	};

	for (k = 0; check[k].f != NULL; k++)
	{
		if (_strcmpr(vars->av[0], check[k].name) == 0)
			break;
	}
	if (check[k].f != NULL)
		check[k].f(vars);
	return (check[k].f);
}

/**
 * new_exit - This function exits program
 *
 * @vars: Represents the variables
 *
 * Return: The function returns void
 *
 */

void new_exit(vars_t *vars)
{
	int root;

	if (_strcmpr(vars->av[0], "exit") == 0 && vars->av[1] != NULL)
	{
		root = _atoi(vars->av[1]);
		if (root == -1)
		{
			vars->root = 2;
			print_error(vars, ": Illegal number: ");
			_puts2(vars->av[1]);
			_puts2("\n");
			free(vars->commands);
			vars->commands = NULL;
			return;
		}
		vars->root = root;
	}
	free(vars->buffer);
	free(vars->av);
	free(vars->commands);
	free_env(vars->env);
	exit(vars->root);
}

/**
 * _env - This function prints the current environment
 *
 * @vars: represents the struct of variables
 *
 * Return: This function returns void.
 *
 */

void _env(vars_t *vars)
{
	unsigned int k;

	for (k = 0; vars->env[k]; k++)
	{
		_puts(vars->env[k]);
		_puts("\n");
	}
	vars->root = 0;
}

/**
 * new_setenv - This function will create a new environment variable
 * or edit an existing variable
 *
 * @vars: This is a pointer to struct of variables
 *
 * Return: This function returns void
 *
 */

void new_setenv(vars_t *vars)
{
	char *v;
	char **y;

	if (vars->av[1] == NULL || vars->av[2] == NULL)
	{
		print_error(vars, ": Incorrect number of arguments\n");
		vars->status = 2;
		return;
	}
	y = find_y(vars->env, vars->av[1]);
	if (y == NULL)
		add_y(vars);
	else
	{
		v = add_value(vars->av[1], vars->av[2]);
		if (v == NULL)
		{
			print_error(vars, NULL);
			free(vars->buffer);
			free(vars->commands);
			free(vars->av);
			free_env(vars->env);
			exit(127);
		}
		free(*y);
		*y = v;
	}
	vars->root = 0;
}

/**
 * new_unsetenv - This function removes an environments variable
 *
 * @vars: This is a pointer to a struct of variables
 *
 * Return: The function returns void
 *
 */

void new_unsetenv(vars_t *vars)
{
	unsigned int h;
	unsigned int v;
	char **y;
	char **nenv;

	if (vars->av[1] == NULL)
	{
		print_error(vars, ": Incorrect number of arguments\n");
		vars->status = 2;
		return;
	}
	y = find_y(vars->env, vars->av[1]);
	if (y == NULL)
	{
		print_error(vars, ": No variable to unset");
		return;
	}
	for (h = 0; vars->env[h] != NULL; h++)
		;
	nenv = malloc(sizeof(char *) * h);
	if (nenv == NULL)
	{
		print_error(vars, NULL);
		vars->status = 127;
		new_exit(vars);
	}

	for (h = 0; vars->env[h] != *y; h++)
		nenv[h] = vars->env[h];
	for (v = h + 1; vars->env[v] != NULL; v++, h++)
		nenv[h] = vars->env[v];
	nenv[h] = NULL;
	free(*y);
	free(vars->env);
	vars->env = nenv;
	vars->root = 0;
}
