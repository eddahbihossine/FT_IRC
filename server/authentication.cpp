/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: onouakch <onouakch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 03:49:12 by onouakch          #+#    #+#             */
/*   Updated: 2023/12/27 03:52:32 by onouakch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.h"

int     ft_sendWelcome( Client *clt, t_server *server )
{
    if (clt->reply(":localhost", RPL_WELCOME, ":Welcome to the IRC server!"))
            return (EXIT_FAILURE);
    if (clt->reply(":localhost", RPL_YOURHOST, ":Your host is " + server->server_name + ", running version 1.0"))
        return (EXIT_FAILURE);
    std::string datetime(server->server_date);
    if (clt->reply(":localhost", RPL_CREATED, ":This server was created " + datetime))
        return (EXIT_FAILURE);
        
    clt->authenticate();
    return (EXIT_SUCCESS);
}

int     ft_nickExists( Client *clt, t_server *server )
{
    std::vector<std::string>::iterator it = std::find(server->nicknames.begin(), server->nicknames.end(), clt->getNickName());
    if (it != server->nicknames.end())
        return (
                    clt->setNickName("*"),
                    clt->reply(":localhost", ERR_NICKNAMEINUSE, clt->getNickName() + " :Nickname is already in use"),
                    EXIT_FAILURE
                );
    server->nicknames.push_back(clt->getNickName());
    return (EXIT_SUCCESS);
}

int    ft_authProcess( t_server *server, Client *clt, std::string buff)
{
    int status;

    buff.pop_back();
    if (buff.find("\r") != std::string::npos)
        buff.pop_back();
    // get data from the new client to authenticate
    if (buff.length() < 5)
        return (clt->reply(":localhost", ERR_NOTREGISTERED, ":You have not registered"), EXIT_SUCCESS);
        
    if (ft_checkCmd(clt, server, buff) == EXIT_FAILURE)
        return (EXIT_SUCCESS);

    // check if the client's data is ready to be authenticated
    status = clt->check_authentification( );
    if (status > 0)
        return (EXIT_FAILURE);
    else if (!status)
    {
        if (ft_nickExists(clt, server))
            return (EXIT_SUCCESS);
        if (ft_sendWelcome(clt, server))
            return (EXIT_FAILURE);
        std::cout << clt->getNickName() << " " << clt->getLoginName() << " " << clt->getRealName() << std::endl;
    }
    return (EXIT_SUCCESS);
}