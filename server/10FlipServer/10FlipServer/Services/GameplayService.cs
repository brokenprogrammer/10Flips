using _10FlipServer.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Services
{
    public class GameplayService
    {

        public User StartGame(Game game)
        {
            ResetDeck(game.Deck);

            User starter = null;
            Card lowest = null;

            foreach (User user in game.Users)
            {
                user.BottomCards = new List<Card>();
                user.TopCards = new List<Card>();
                for (int i = 0; i < 3; i++)
                {
                    Card card1 = game.Deck.Cards.Pop();
                    user.BottomCards.Add(card1);
                    Card card2 = game.Deck.Cards.Pop();
                    user.TopCards.Add(card2);
                    Card card3 = game.Deck.Cards.Pop();
                    user.Hand.Add(card3);
                    if (lowest == null || card3.Value < lowest.Value || (card3.Value == lowest.Value && card3.Suit < lowest.Suit))
                    {
                        lowest = card3;
                        starter = user;
                    }
                }
            }

            return starter;
        }

        public void ResetDeck(Deck deck)
        {
            List<Card> cards = new List<Card>();
            for (CardType type = CardType.TWO_OF_HEARTS; type <= CardType.ACE_OF_SPADES; type++)
            {
                cards.Add(new Card(type));
            }
            cards = cards.OrderBy(a => Guid.NewGuid()).ToList();

            deck.Cards = new Stack<Card>();
            foreach (Card card in cards)
            {
                deck.Cards.Push(card);
            }
        }
    }
}
