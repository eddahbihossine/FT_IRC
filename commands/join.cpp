/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heddahbi <heddahbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/30 11:07:43 by onouakch          #+#    #+#             */
/*   Updated: 2024/01/18 01:47:29 by heddahbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/irc.h"
#include "../models/Channel.hpp"
#include "../models/Client.hpp"
#include <sstream>




int		ft_joinChannel( t_server *server, Client *clt, std::string target, std::string key)
{
	std::cout << "[" << target << " => " << key << "]" << std::endl;
	char c = target[0];
	if (c != '#' && c != '&')
		return (
			clt->reply(server->host_name, ERR_NOSUCHCHANNEL, target + " :No such channel"),
			EXIT_FAILURE
		);
	std::map<std::string, Channel*>::iterator it = server->channels.find(target);
	if (it != server->channels.end())
	{
		if (it->second->users.find(clt->getNickName()) == it->second->users.end())
		{
			it->second->users.insert(std::pair<std::string, Client*>(clt->getNickName(), clt));
			std::map<std::string, Client *>::iterator c_it = it->second->users.begin();
			while (c_it != it->second->users.end())
			{
				std::string msg = ":" + clt->getNickName() + "!" + clt->getLoginName() + "@" + server->host_name + " JOIN " + target + " \r\n";
				std::cout << "-*- " << msg;
				send(c_it->second->getSocket(), msg.c_str(), msg.size(), 0);
				c_it++;
			}
			int RPL = (it->second->topic == " :No topic is set") ? RPL_NOTOPIC : RPL_TOPIC;
			clt->reply(server->host_name, RPL, target + " :" + it->second->topic);
			clt->reply(server->host_name, RPL_NAMREPLY, "= " + target + " :" + it->second->u_list());
			clt->reply(server->host_name, RPL_ENDOFNAMES, target + " :End of /NAMES list");
		}
	}
	else
	{
		Channel *new_chnl = new Channel(target, key, clt);
		server->channels.insert(std::pair<std::string, Channel*>(target, new_chnl));
		std::string msg = ":" + clt->getNickName() + "!~" + clt->getLoginName() + "@" + server->host_name + " JOIN " + target + " \r\n";
		std::cout << "-*- " << msg;
		send(clt->getSocket(), msg.c_str(), msg.size(), 0);
		clt->reply(server->host_name, RPL_NOTOPIC, target + " :No topic is set");
		clt->reply(server->host_name, RPL_NAMREPLY, "= " + target + " :@" + clt->getNickName());
		clt->reply(server->host_name, RPL_ENDOFNAMES, target + " :End of /NAMES list.");
	}
	return (EXIT_SUCCESS);
}

int     ft_joinCmd( t_server *server, Client *clt, std::vector<std::string> &items )
{
	std::string target, key;
	size_t size = items.size();
	std::stringstream ss_targets(items[1]);
	std::stringstream ss_keys((size == 3) ? items[2] : "");
	
	if (size < 2 || size > 3)
		return (
			clt->reply(server->host_name, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters"),
			EXIT_FAILURE
		);
	while (std::getline(ss_targets, target, ','))
	{
		std::getline(ss_keys, key, ',');
		ft_joinChannel(server, clt, target, key);
		key.clear();
	}
    return (EXIT_SUCCESS);
}